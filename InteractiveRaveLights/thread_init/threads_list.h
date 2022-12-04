#ifndef THREADS_LIST_H
#define THREADS_LIST_H
#include "pthread.h"
#include "RaveLightsRuntime/rave_lights_runtime.h"
#define THREAD_LIST                                                 \
    {                                                               \
        {rave_light_runtime_main, "Rave Light runtime main", NULL}, \
    }

#define NUM_THREADS 2

#endif