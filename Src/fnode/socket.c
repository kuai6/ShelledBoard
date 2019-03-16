#include "socket.h"
#include <Ethernet/socket.h>

fnet_socket_t const FNET_INVALID_SOCKET = (fnet_socket_t)~0u;

static uint8_t sn = 0;

fnet_socket_t fnet_socket_create(uint16_t port)
{
    fnet_socket_t sock_id = sn++;
    int8_t result = socket(sock_id, Sn_MR_UDP, port, SF_IO_NONBLOCK);
    if (result < 0)
        return FNET_INVALID_SOCKET;
    return sock_id;
}

bool fnet_socket_sendto(fnet_socket_t sock, const uint8_t *buf, uint16_t len, fnet_address_t const *addr, uint16_t port)
{
    int8_t result = sendto(sock, (uint8_t *)buf, len, (uint8_t *)addr, port);
    return result >= 0;
}
