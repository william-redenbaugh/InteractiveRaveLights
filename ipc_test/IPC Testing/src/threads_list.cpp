#include "ipc_message_publishqueue.h"
#include "ipc_thread.h"
#include "test.h"

#include "THREAD_INIT/threads_init.h"
task_init_descriptor_t THREAD_LIST[] =                                                                                    \
    {                                                                                                       \
        {uart_ipc_publish_thread, uart_ipc_publish_init, "UART IPC Publish", 512, 1, NULL, NULL},          \
        {uart_ipc_consume_thread, uart_ipc_consume_thread_init, "UART IPC Subscribe", 512, 1, NULL, NULL}, \            
        {test_thread, NULL, "test thread", 512, NULL, NULL} \                             
    };

int NUM_THREADS  = sizeof(THREAD_LIST)/sizeof(task_init_descriptor_t);