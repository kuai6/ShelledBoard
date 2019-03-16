#pragma once
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void psleep(uint32_t msec);

typedef void (*ptask_fn_t) (void *);
typedef void * ptask_t;

ptask_t ptask_start(char const *name, ptask_fn_t fn, void *argument);
void ptask_terminate(ptask_t task);

#ifdef __cplusplus
}
#endif
