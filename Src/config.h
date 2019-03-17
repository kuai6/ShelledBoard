#pragma once
#include "fnode/limits.h"
#include "fnode/bank.h"

extern const char SN[FSN_LENGTH];

enum
{
    FBANK_TADC_ID = 0,
    FBANK_COCL_ID = 1,
    FBANK_RELY_ID = 2,
    FBANKS_NUM = 3
};

extern const fbank_info BANKS[FBANKS_NUM];
