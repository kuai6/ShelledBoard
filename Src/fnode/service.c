#include "service.h"
#include "commands.h"
#include "socket.h"
#include "platform.h"
#include <string.h>
#include <Internet/TFTP/netutil.h>

#define FNODE_BROADCAST_ADDR    { 255, 255, 255, 255 }
#define FNODE_BROADCAST_PORT    9999
#define FNODE_HELLO_FREQ        5000    // msec

typedef enum
{
    FSVC_INIT = 0,
    FSVC_NOTIFY_STATUS,
    FSVC_PROCESS_COMMANDS,
    FSVC_STOPPED
} fnode_service_state_t;

struct fnode_service
{
    volatile int                ref_counter;
    char                        sn[FSN_LENGTH];
    uint32_t                    banks_num;
    fbank_info const           *banks;
    fnet_socket_t               socket;
    fnode_service_state_t       state;
    fnode_service_handler_t     handler;
    uint32_t                    keepalive;
    uint32_t                    resp_freq;
    fnet_address_t              server;
    uint16_t                    server_port;
    uint8_t                     buffer[1024];
};

static fnode_service_t service = {};

static fnode_service_state_t fnode_service_init_handler(fnode_service_t *svc);
static void                  fnode_service_hello(fnode_service_t *svc);
static fcmd_id               fnode_service_recv_cmd(fnode_service_t *svc);
static fnode_service_state_t fnode_service_notify_status(fnode_service_t *svc);
static fnode_service_state_t fnode_service_process_commands(fnode_service_t *svc);
static fbank_info const *    fnode_service_bank(fnode_service_t *svc, uint8_t bank_id);

fnode_service_t *fnode_service_create(char const sn[FSN_LENGTH], uint32_t banks_num, fbank_info const banks[])
{
    memset(&service, 0, sizeof service);

    service.ref_counter = 1;
    memcpy(service.sn, sn, FSN_LENGTH);
    service.banks_num = banks_num;
    service.banks = banks;
    service.state = FSVC_INIT;
    service.socket = fnet_socket_create(12345);

    return &service;
}

fnode_service_t *fnode_service_retain(fnode_service_t *svc)
{
    if (svc)
        svc->ref_counter++;
    return svc;
}

void fnode_service_release(fnode_service_t *svc)
{
    if (svc)
    {
        if (!--svc->ref_counter)
        {
            svc->state = FSVC_STOPPED;
        }
    }
}

void fnode_service_update(fnode_service_t *svc)
{
    switch(svc->state)
    {
        case FSVC_INIT:
        {
            svc->state = fnode_service_init_handler(svc);
            break;
        }

        case FSVC_NOTIFY_STATUS:
        {
            svc->state = fnode_service_notify_status(svc);
            break;
        }

        case FSVC_PROCESS_COMMANDS:
        {
            svc->state = fnode_service_process_commands(svc);
            break;
        }
    }
}

fnode_service_state_t fnode_service_init_handler(fnode_service_t *svc)
{
    // broadcast helllo message
    fnode_service_hello(svc);

    // wait response
    psleep(FNODE_HELLO_FREQ);

    // try to read
    if (fnode_service_recv_cmd(svc) == FCMD_CONF)
        return FSVC_NOTIFY_STATUS;

    return FSVC_INIT;
}

fcmd_id fnode_service_recv_cmd(fnode_service_t *svc)
{
    fnet_address_t addr = {};
    uint16_t port = 0;

    int32_t const len = fnet_socket_recvfrom(svc->socket, svc->buffer, sizeof svc->buffer, &addr, &port, false);
    if (len <= 0)
        return FCMD_NVLD;

    uint32_t const cmd_id = *(uint32_t const*)svc->buffer;

    switch(cmd_id)
    {
        case FCMD_CONF:
        {
            fcmd_conf const *cmd = (fcmd_conf const*)svc->buffer;
            svc->keepalive = cmd->keepalive;
            svc->resp_freq =cmd->resp_freq;
            svc->server = addr;
            svc->server_port = port;
            return FCMD_CONF;
        }

        case FCMD_PONG:
        {
            return FCMD_PONG;
        }

        case FCMD_DATA:
        {
            fcmd_data const *cmd = (fcmd_data const*)svc->buffer;
            fbank_state banks[cmd->banks_num];

            uint8_t const *pbank = cmd->banks;

            for(uint16_t i = 0; i < cmd->banks_num; ++i)
            {
                uint8_t const bank_id = *pbank;
                fbank_info const *bank_inf = fnode_service_bank(svc, bank_id);
                if (!bank_inf)      // Unknown bank id
                    return FCMD_NVLD;

                banks[i].id = bank_id;
                banks[i].pins8 = ++pbank;
                pbank += fpins_vector_size(bank_inf);
            }

            svc->handler(cmd->banks_num, banks);
            return FCMD_DATA;
        }

        default:
            break;
    }

    return FCMD_NVLD;
}

static void pingTask(fnode_service_t *svc)
{
    for(;;)
    {
        psleep(svc->keepalive);
        fcmd_ping cmd = { FCMD_PING };
        memcpy(cmd.sn, svc->sn, sizeof svc->sn);
        fnet_socket_sendto(svc->socket, (const uint8_t *)&cmd, sizeof cmd, &svc->server, svc->server_port);
    }
}

fnode_service_state_t fnode_service_notify_status(fnode_service_t *svc)
{
    ptask_start("pingTask", (ptask_fn_t)pingTask, svc);
    return FSVC_PROCESS_COMMANDS;
}

fnode_service_state_t fnode_service_process_commands(fnode_service_t *svc)
{
    if (fnode_service_recv_cmd(svc) == FCMD_NVLD)
        psleep(100);
    return FSVC_PROCESS_COMMANDS;
}

static void fnode_service_hello(fnode_service_t *svc)
{
    // get broadcast address
    fnet_address_t const broadcast_addr = FNODE_BROADCAST_ADDR;

    // prepare command buffer
    uint16_t const cmd_buf_size = sizeof(fcmd_hello) + sizeof(fbank_info) * svc->banks_num;
    uint8_t cmd_buf[cmd_buf_size];

    fcmd_hello *cmd = (fcmd_hello *)cmd_buf;
    fbank_info *banks = (fbank_info *)(cmd_buf + sizeof(fcmd_hello));

    cmd->cmd       = FCMD_HELO;
    cmd->banks_num = svc->banks_num;
    memcpy(cmd->sn, svc->sn,    sizeof svc->sn);
    memcpy(banks,   svc->banks, sizeof(fbank_info) * svc->banks_num);

    // broadcast hello message
    fnet_socket_sendto(svc->socket, cmd_buf, cmd_buf_size, &broadcast_addr, FNODE_BROADCAST_PORT);
}

void fnode_service_notify_state(fnode_service_t *svc, uint32_t banks_num, fbank_state const banks[])
{
    if (svc)
    {
        fcmd_status cmd = { FCMD_DATA };
        memcpy(cmd.sn, svc->sn, sizeof svc->sn);
        cmd.banks_num = banks_num;

        uint8_t *bank = cmd.banks;

        for(uint32_t i = 0; i < banks_num; ++i)
        {
            fbank_info const *bank_inf = fnode_service_bank(svc, banks[i].id);
            if (!bank_inf)      // Unknown bank id
                return;

            // Save bank ID
            if (FMAX_DATA_LENGTH - (bank - cmd.banks) < sizeof banks[i].id)
                return;     // No space for banks state
            *bank = banks[i].id;
            ++bank;

            uint16_t const pins_vector_size = fpins_vector_size(bank_inf);

            // Save bank data
            if (FMAX_DATA_LENGTH - (bank - cmd.banks) < pins_vector_size)
                return;     // No space for banks state
            memcpy(bank, banks[i].pins8, pins_vector_size);
            bank += pins_vector_size;
        }

        uint16_t const size = bank - (uint8_t const *)&cmd;
        fnet_socket_sendto(svc->socket, (const uint8_t *)&cmd, size, &svc->server, svc->server_port);
    }
}

void fnode_service_reg_handler(fnode_service_t *svc, fnode_service_handler_t handler)
{
    if (svc)
        svc->handler = handler;
}

static fbank_info const * fnode_service_bank(fnode_service_t *svc, uint8_t bank_id)
{
    for(uint32_t i = 0; i < svc->banks_num; ++i)
    {
        if (svc->banks[i].id == bank_id)
            return svc->banks + i;
    }
    return 0;
}
