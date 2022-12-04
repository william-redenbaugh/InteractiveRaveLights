#ifndef THREADS_LIST_H
#define THREADS_LIST_H
#include "pthread.h"
#include "led_matrix_runtime/led_matrix_runtime.h"

#define THREAD_LIST                                             \
    {                                                           \
        {led_matrix_thread, "LED Matrix thread", NULL},         \
            {led_trigger_thread, "Event Handler Thread", NULL}, \
    }

#define NUM_THREADS 2

#endif