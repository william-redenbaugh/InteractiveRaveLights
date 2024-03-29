#ifndef THREADS_LIST_H
#define THREADS_LIST_H
#include "Arduino.h"
#include "FreeRTOS.h"
#include "serial_communication_thread.h"
#include "ipc_thread.h"

#define THREAD_LIST                                                                                         \
    {                                                                                                       \
        {uart_ipc_publish_thread, uart_ipc_publish_init, "UART IPC Publish", 2048, 1, NULL, NULL},          \
        {uart_ipc_consume_thread, uart_ipc_consume_thread_init, "UART IPC Subscribe", 2048, 1, NULL, NULL}  \
    }

#define NUM_THREADS 2

#endif