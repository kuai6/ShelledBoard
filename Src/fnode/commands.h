#pragma once
#include "limits.h"
#include "bank.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define FCMD(a, b, c, d) a | (b << 8) | (c << 16) | (d << 24)

typedef enum
{
    FCMD_NVLD = FCMD('N', 'V', 'L', 'D'),
    FCMD_HELO = FCMD('H', 'E', 'L', 'O'),
    FCMD_CONF = FCMD('C', 'O', 'N', 'F'),
    FCMD_PING = FCMD('P', 'I', 'N', 'G'),
    FCMD_PONG = FCMD('P', 'O', 'N', 'G'),
    FCMD_DATA = FCMD('D', 'A', 'T', 'A')
} fcmd_id;

#define FCMD_CHARS(cmd) (cmd & 0xFF), ((cmd >> 8) & 0xFF), ((cmd >> 16) & 0xFF), ((cmd >> 24) & 0xFF)

typedef struct __attribute__ ((packed))
{
    fcmd_id     cmd;
    char        sn[FSN_LENGTH];
    uint8_t     banks_num;
    //fbank_info  banks[banks_num];
} fcmd_hello;

typedef struct __attribute__ ((packed))
{
    fcmd_id     cmd;
    uint32_t    keepalive;  // msec
    uint32_t    resp_freq;  // msec
} fcmd_conf;

typedef struct
{
    fcmd_id     cmd;
    char        sn[FSN_LENGTH];
} fcmd_ping;

typedef struct
{
    fcmd_id     cmd;
} fcmd_pong;

typedef struct
{
    fcmd_id     cmd;
    char        sn[FSN_LENGTH];
    char        data[FMAX_DATA_LENGTH];
} fcmd_node_status;

typedef struct
{
    fcmd_id     cmd;
    char        data[FMAX_DATA_LENGTH];
} fcmd_data;

/*
    Device              Controller
    HELO     ->
        SN[8]
        BANKS_NUM[1]
        'TADC', ID[1], PINS[1]
        'COCL', ID[1], PINS[1]
        'RELY', ID[1], PINS[1]
        'TDAC', ID[1], PINS[1]
             <-         CONF
                            KEEPALIVE[4]     (mSec)
                            MAX_RESP_FREQ[4] (mSec)
    PING     ->
        SN[8]
             <-         PONG
    DATA     ->
        SN[8]
        VAL[]
             <-         DATA
                            VAL[]
*/

#ifdef __cplusplus
}
#endif
