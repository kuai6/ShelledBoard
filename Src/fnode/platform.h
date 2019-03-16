#pragma once
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void psleep(uint32_t msec);

typedef void (*pthread_fn_t) (void *);
typedef void * pthread_t;

pthread_t pthread_start(char const *name, pthread_fn_t fn, void *argument);
void      pthread_terminate(pthread_t task);

#ifdef __cplusplus
}
#endif
