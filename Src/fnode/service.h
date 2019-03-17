#pragma once
#include "limits.h"
#include "bank.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct fnode_service fnode_service_t;

typedef void (*fnode_service_data_handler_t)(uint32_t banks_num, fbank_state const banks[]);
typedef void (*fnode_service_dget_handler_t)(uint8_t bank_id);

fnode_service_t *fnode_service_create(char const sn[FSN_LENGTH], uint32_t banks_num, fbank_info const banks[]);
fnode_service_t *fnode_service_retain(fnode_service_t *);
void             fnode_service_release(fnode_service_t *);
void             fnode_service_notify_state(fnode_service_t *, uint32_t banks_num, fbank_state const banks[]);
void             fnode_service_reg_data_handler(fnode_service_t *, fnode_service_data_handler_t);
void             fnode_service_reg_dget_handler(fnode_service_t *, fnode_service_dget_handler_t);
void             fnode_service_update(fnode_service_t *);

#ifdef __cplusplus
}
#endif
