#pragma once
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define FBANK(a, b, c, d) a | (b << 8) | (c << 16) | (d << 24)

typedef enum
{
    FBANK_TADC = FBANK('T', 'A', 'D', 'C'),
    FBANK_TDAC = FBANK('T', 'D', 'A', 'C'),
    FBANK_COCL = FBANK('C', 'O', 'C', 'L'),
    FBANK_RELY = FBANK('R', 'E', 'L', 'Y')
} fbank_type;

#define FBANK_CHARS(bnk) (bnk & 0xFF), ((bnk >> 8) & 0xFF), ((bnk >> 16) & 0xFF), ((bnk >> 24) & 0xFF)

typedef struct __attribute__ ((packed))
{
    char    type[FBANK_TYPE_LENGTH];
    uint8_t id;
    uint8_t pins;
} fbank_info;

#ifdef __cplusplus
}
#endif
