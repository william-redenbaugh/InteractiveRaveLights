#ifndef THREADS_LIST_H
#define THREADS_LIST_H
#include "pthread.h"
#include "adc_runtime/adc_runtime_main.h"
#include "fft_runtime/fft_runtime.h"
#include "fft_runtime/fft_copy_thread.h"
#include "matrix_animation_thread/led_matrix_runtime.h"
#include "beat_detection/beat_detection.h"
#include "led_strip_threads/strip_modules.h"

#define THREAD_LIST                                                                         \
    {                                                                                       \
        {adc_runtime_thread, adc_init_func, "Rave Light runtime main", NULL},               \
        {fft_primary_task, fft_module_init, "FFT Processing Main task", NULL},              \
        {fft_copy_thread, init_fft_copy_mod, "FFT Processing Copy task", NULL},             \
        {beat_detection_thread, beat_detection_module_init, "Beat Detection task", NULL },  \
        {led_matrix_runtime, led_matrix_init, "LED matrix runtime", NULL },                 \
        {led_strip_update_module_thread, strip_update_module_init, "Strip update, NULL"},   \
        {led_strip_thread_one, led_strip_thread_one_init, "Strip Thread 1", NULL},          \
        {led_strip_thread_two, led_strip_thread_two_init, "Strip Thread 2", NULL},          \
    }

#define NUM_THREADS 8

#endif