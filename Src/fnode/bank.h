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

typedef struct __attribute__ ((packed))
{
    fbank_type  type;
    uint8_t     id;
    uint8_t     pins;
} fbank_info;

typedef struct
{
    uint8_t  id;
    union
    {
        uint8_t  const *pins8;
        uint16_t const *pins16;
    };
} fbank_state;

uint8_t  fpin_size(fbank_type bank_type);
uint16_t fpins_vector_size(fbank_info const *inf);

#ifdef __cplusplus
}
#endif
