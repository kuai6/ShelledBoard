#include "bank.h"

uint8_t fpin_size(fbank_type bank_type)
{
    switch(bank_type)
    {
        case FBANK_TADC:    return 16;
        case FBANK_TDAC:    return 16;
        case FBANK_COCL:    return 1;
        case FBANK_RELY:    return 1;
    }
    // Unknown pin
    return 0;
}

uint16_t fpins_vector_size(fbank_info const *inf)
{
    uint16_t const size = fpin_size(inf->type) * inf->pins;
    return (size + 7) / 8;
}

