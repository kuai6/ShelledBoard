#include "platform.h"
#include <cmsis_os.h>

void psleep(uint32_t msec)
{
    osDelay(msec);
}

pthread_t pthread_start(char const *name, pthread_fn_t fn, void *argument)
{
    const osThreadDef_t os_thread_def =
    { name, fn, osPriorityNormal, 0, 128 };
    return osThreadCreate(&os_thread_def, argument);
}

void pthread_terminate(pthread_t task)
{
    osThreadTerminate(task);
}
