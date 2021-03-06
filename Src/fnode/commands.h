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
    FCMD_DATA = FCMD('D', 'A', 'T', 'A'),
    FCMD_DGET = FCMD('D', 'G', 'E', 'T')
} fcmd_id;

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
    uint8_t     banks_num;
    uint8_t     banks[FMAX_DATA_LENGTH];
} fcmd_status;

typedef struct
{
    fcmd_id     cmd;
    uint8_t     banks_num;
    uint8_t     banks[FMAX_DATA_LENGTH];
} fcmd_data;

typedef struct __attribute__ ((packed))
{
    fcmd_id     cmd;
    uint8_t     bank_id;
} fcmd_dget;

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
        BANKS_NUM[1]
        ID[1], PINS[...]
        ID[1], PINS[...]
        ID[1], PINS[...]
             <-         DATA
                            BANKS_NUM[1]
                            ID[1], PINS[...]
                            ID[1], PINS[...]
                            ID[1], PINS[...]
             <-         DGET
                            ID[1]
    DATA     ->
        SN[8]
        BANKS_NUM[1]
        ID[1], PINS[...]
*/

#ifdef __cplusplus
}
#endif
