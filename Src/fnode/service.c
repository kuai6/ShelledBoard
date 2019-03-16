#include "service.h"
#include "commands.h"
#include "socket.h"
#include "platform.h"
#include <string.h>
#include <Internet/TFTP/netutil.h>

//#include <stdlib.h>
//#include <unistd.h>
//#include <sys/time.h>

#define FNODE_BROADCAST_ADDR    { 255, 255, 255, 255 }
#define FNODE_BROADCAST_PORT    9999
#define FNODE_HELLO_FREQ        1000    // msec

typedef enum
{
    FSVC_INIT = 0,
    FSVC_PROCESS_COMMANDS,
    FSVC_NOTIFY_STATUS,
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
    fnode_service_cmd_handler_t data_handler;
    uint32_t                    keepalive;
    uint32_t                    resp_freq;
    fnet_address_t              server;
    uint16_t                    server_port;
    size_t                      last_cmd_time;
};

static fnode_service_t service = {};
static uint8_t buffer[1024];

static fnode_service_state_t fnode_service_init_handler(fnode_service_t *svc);
static fcmd_id               fnode_service_recv_cmd(fnode_service_t *svc);

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
    if (!svc)
        return;

    switch(svc->state)
    {
        case FSVC_INIT:
        {
            svc->state = fnode_service_init_handler(svc);
            if (svc->state == FSVC_INIT)
                psleep(FNODE_HELLO_FREQ);
            break;
        }

        case FSVC_NOTIFY_STATUS:
        {
//            svc->state = fnode_service_notify_status(svc);
            break;
        }

        case FSVC_PROCESS_COMMANDS:
        {
//            svc->state = fnode_service_process_commands(svc);
            break;
        }
    }
}

fnode_service_state_t fnode_service_init_handler(fnode_service_t *svc)
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

    if (fnode_service_recv_cmd(svc) == FCMD_CONF)
        return FSVC_NOTIFY_STATUS;

    return FSVC_INIT;
}

fcmd_id fnode_service_recv_cmd(fnode_service_t *svc)
{
    fnet_address_t addr = {};
    uint16_t port = 0;

    int32_t const len = fnet_socket_recvfrom(svc->socket, buffer, sizeof buffer, &addr, &port, false);
    if (len <= 0)
        return FCMD_NVLD;

    uint32_t const cmd_id = *(uint32_t const*)buffer;

    switch(cmd_id)
    {
        case FCMD_CONF:
        {
            fcmd_conf const *cmd = (fcmd_conf const*)buffer;
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
            break;
            //svc->data_handler(cmd, buf + FCMD_ID_LENGTH, read_len - FCMD_ID_LENGTH);
            return FCMD_DATA;
        }

        default:
            break;
    }

    return FCMD_NVLD;
}

/*
static fnode_service_state_t fnode_service_notify_status(fnode_service_t *svc);
static fnode_service_state_t fnode_service_process_commands(fnode_service_t *svc);
static size_t                fnode_service_time();

static void fnode_service_create_iface_sockets(fnode_service_t *svc)
{
    svc->ifaces_num = fnet_socket_bind_all(FNET_SOCK_DGRAM, svc->ifaces, sizeof svc->ifaces / sizeof *svc->ifaces, FNET_SOCK_BROADCAST);
}

static void fnode_service_close_iface_sockets(fnode_service_t *svc)
{
    for (size_t i = 0; i < svc->ifaces_num; ++i)
        fnet_socket_close(svc->ifaces[i]);
}

void fnode_service_notify_state(fnode_service_t *svc, char const state[FMAX_DATA_LENGTH], uint32_t size)
{
    if (svc)
    {
        size_t time_now = fnode_service_time();

        fcmd_node_status cmd = {{ FCMD_CHARS(FCMD_DATA) }};
        memcpy(cmd.sn,       svc->sn,       sizeof svc->sn);
        memcpy(cmd.data,     state,         size > FMAX_DATA_LENGTH ? FMAX_DATA_LENGTH : size);

        if (fnet_socket_sendto(svc->socket, (const char *)&cmd, sizeof cmd, &svc->server))
            svc->last_cmd_time = time_now;
    }
}

void fnode_service_reg_handler(fnode_service_t *svc, fnode_service_cmd_handler_t handler)
{
    if (svc)
        svc->data_handler = handler;
}

fnode_service_state_t fnode_service_notify_status(fnode_service_t *svc)
{
    size_t time_now = fnode_service_time();
    size_t time_diff = time_now - svc->last_cmd_time;

    if (time_diff >= svc->keepalive)
    {
        fcmd_ping cmd = {{ FCMD_CHARS(FCMD_PING) }};
        memcpy(cmd.sn,       svc->sn,       sizeof svc->sn);
        fnet_socket_sendto(svc->socket, (const char *)&cmd, sizeof cmd, &svc->server);
        svc->last_cmd_time = time_now;
    }

    return FSVC_PROCESS_COMMANDS;
}

fnode_service_state_t fnode_service_process_commands(fnode_service_t *svc)
{
    fnet_socket_t rs, es;
    size_t rs_num = 0, es_num = 0;

    size_t time_now = fnode_service_time();
    size_t time_elapsed = time_now - svc->last_cmd_time;
    size_t time_wait = time_elapsed < svc->keepalive ? svc->keepalive - time_elapsed : 1;

    if (fnet_socket_select(&svc->socket, 1, &rs, &rs_num, &es, &es_num, time_wait) && rs_num)
        fnode_service_recv_cmd(svc);

    return FSVC_NOTIFY_STATUS;
}

size_t fnode_service_time()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}
*/
