#ifndef THREADS_LIST_H
#define THREADS_LIST_H
#include "Arduino.h"
#include "FreeRTOS.h"
#include "serial_communication_thread.h"

#define THREAD_LIST                                                                                        \
    {                                                                                                      \
        {serial_commuincation_thread, serial_communication_setup, "Main Statemachine", 2048, 1, NULL, NULL}, \
    }

#define NUM_THREADS 1

#endif