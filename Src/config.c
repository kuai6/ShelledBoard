#include "config.h"

const char SN[FSN_LENGTH] = "a19d487e";

const fbank_info BANKS[FBANKS_NUM] =
{
    { FBANK_CHARS(FBANK_TADC), 0, 1 },
    { FBANK_CHARS(FBANK_COCL), 1, 3 },
    { FBANK_CHARS(FBANK_RELY), 2, 3 }
};
