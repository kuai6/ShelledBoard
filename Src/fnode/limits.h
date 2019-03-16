#pragma once

#ifdef __cplusplus
extern "C" {
#endif

enum
{
    FSN_LENGTH                  = 8,        // Serial number length
    FBANK_TYPE_LENGTH           = 4,        // Bank type length
    FCMD_ID_LENGTH              = 4,        // Command type length
    FMAX_ACCEPT_CONNECTIONS     = 10,       // Maximum number of accepted connections
    FMAX_ADDR                   = 1024,     // Max address length
    FMAX_DATA_LENGTH            = 1024      // Max data length
};

#ifdef __cplusplus
}
#endif
