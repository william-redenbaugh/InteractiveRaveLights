#ifndef IPC_MESSAGE_PUBLISHQUEUE_H
#define IPC_MESSAGE_PUBLISHQUEUE_H
#include "ipc.h"
#include "pthread.h"

typedef enum ipc_message_callback_status
{
    IPC_MESSAGE_COMPLETE_SUCCESS,
    IPC_MESSAGE_COMPLETE_FAIL,
} ipc_message_callback_status_t;

typedef struct ipc_message_callback
{
    ipc_message_callback_status_t ipc_status;
} ipc_message_ret_t;

typedef void(*ipc_message_complete_callback_t)(ipc_message_ret_t);

typedef struct ipc_message_node{
    ipc_message_header_t message_header;
    uint8_t *buffer_ptr;
    ipc_message_complete_callback_t callback_func;
} ipc_message_node_t;

typedef struct ipc_message_publish_module{
    ipc_message_node_t *head_node;
    pthread_mutex_t ipc_message_node_muttx;
}ipc_message_publish_module_t;

extern ipc_message_publish_module_t ipc_publush_queue_module;

bool ipc_publish_message();

#endif