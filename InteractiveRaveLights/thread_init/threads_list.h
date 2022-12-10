#ifndef THREADS_LIST_H
#define THREADS_LIST_H
#include "pthread.h"
#include "adc_runtime/adc_runtime_main.h"
#include "fft_runtime/fft_runtime.h"
#include "matrix_animation_thread/led_matrix_runtime.h"
#define THREAD_LIST                                                 \
    {                                                               \
        {adc_runtime_thread, "Rave Light runtime main", NULL},      \
        {led_matrix_runtime, "LED matrix runtime", NULL },          \
        {fft_primary_task, "FFT Processing Main task", NULL}        \
    }

#define NUM_THREADS 3

#endif