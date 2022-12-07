#ifndef THREADS_LIST_H
#define THREADS_LIST_H
#include "pthread.h"
#include "RaveLightsRuntime/rave_lights_runtime.h"
#include "matrix_animation_thread/led_matrix_runtime.h"
#define THREAD_LIST                                                 \
    {                                                               \
        {rave_light_runtime_main, "Rave Light runtime main", NULL}, \
        {led_matrix_runtime, "LED matrix runtime", NULL }\
    }

#define NUM_THREADS 2

#endif