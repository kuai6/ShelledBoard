#include "platform.h"
#include <cmsis_os.h>

void psleep(uint32_t msec)
{
    osDelay(msec);
}
