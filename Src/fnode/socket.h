#pragma once
#include "ip_address.h"
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef uint8_t fnet_socket_t;

extern fnet_socket_t const FNET_INVALID_SOCKET;

fnet_socket_t   fnet_socket_create(uint16_t port);
bool            fnet_socket_sendto(fnet_socket_t sock, const uint8_t *buf, uint16_t len, fnet_address_t const *addr, uint16_t port);

#ifdef __cplusplus
}
#endif
