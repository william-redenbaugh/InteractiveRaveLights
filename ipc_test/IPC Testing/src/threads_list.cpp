#include "MODULES/TeensyOSIPC/ipc_message_publishqueue.h"
#include "MODULES/TeensyOSIPC/ipc_thread.h"
#include "test.h"

#include "THREAD_INIT/threads_init.h"

#define IPC_PUBLISH_STACK_SIZE 1024
uint8_t ipc_publish_stack[IPC_PUBLISH_STACK_SIZE];

#define IPC_CONSUME_STACK_SIZE 1024
uint8_t ipc_consume_stack[IPC_CONSUME_STACK_SIZE];

#define TEST_THREAD_STACK_SIZE 512
uint8_t test_thread_stack[TEST_THREAD_STACK_SIZE];

task_init_descriptor_t THREAD_LIST[] =                                                                                                              \
    {                                                                                                                                               \
        {uart_ipc_publish_thread, uart_ipc_publish_init, "UART IPC Publish", IPC_CONSUME_STACK_SIZE, ipc_publish_stack, 1, NULL, NULL},             \
        {uart_ipc_consume_thread, uart_ipc_consume_thread_init, "UART IPC Subscribe", IPC_CONSUME_STACK_SIZE, ipc_consume_stack, 1, NULL, NULL},    \            
        {test_thread, NULL, "test thread", TEST_THREAD_STACK_SIZE, test_thread_stack, NULL, NULL} \                             
    };

int NUM_THREADS  = sizeof(THREAD_LIST)/sizeof(task_init_descriptor_t);