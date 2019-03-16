#include "platform.h"
#include <cmsis_os.h>

void psleep(uint32_t msec)
{
    osDelay(msec);
}

ptask_t ptask_start(char const *name, ptask_fn_t fn, void *argument)
{
    const osThreadDef_t os_thread_def =
    { name, fn, osPriorityNormal, 0, 128 };
    return osThreadCreate(&os_thread_def, argument);
}

void ptask_terminate(ptask_t task)
{
    osThreadTerminate(task);
}
