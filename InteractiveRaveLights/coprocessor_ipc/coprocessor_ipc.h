#ifndef _COPROCESSOR_IPC
#define _COPROCESSOR_IPC

#include "stdio.h"
#include "../stripCoProcessor/include/coprocessor_ipc_shared.h"

void coprocessor_ipc_init(void *params);
void coprocessor_ipc_thread(void *params);

void coprocessor_ipc_led_strip_update_all_threads(void);
void coprocesor_ipc_led_strip_set(led_strip_t strip, int n, uint8_t r, uint8_t g, uint8_t b);
#endif