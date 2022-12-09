#ifndef THREADS_LIST_H
#define THREADS_LIST_H
#include "pthread.h"
#include "adc_runtime/adc_runtime_main.h"
#include "matrix_animation_thread/led_matrix_runtime.h"
#define THREAD_LIST                                                 \
    {                                                               \
        {adc_runtime_thread, "Rave Light runtime main", NULL}, \
        {led_matrix_runtime, "LED matrix runtime", NULL }\
    }

#define NUM_THREADS 2

#endif