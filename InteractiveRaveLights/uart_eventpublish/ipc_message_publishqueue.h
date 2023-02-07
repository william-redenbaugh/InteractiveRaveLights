#ifndef IPC_MESSAGE_PUBLISHQUEUE_H
#define IPC_MESSAGE_PUBLISHQUEUE_H
#include "ipc.h"
#include "pthread.h"

#define IPC_QUEUE_MAX_NUM_ELEMENTS 128

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
    ipc_message_node_t *node_list;
    size_t max_size;
    size_t current_size;
    size_t current_pos;

    pthread_mutex_t ipc_message_node_muttx;
}ipc_message_publish_module_t;

extern ipc_message_publish_module_t *ipc_publush_queue_module;

bool _ipc_publish_message(ipc_message_publish_module_t *module, ipc_message_node_t node);
bool ipc_publish_message(ipc_message_node_t node);

/**
 *
*/
ipc_message_publish_module_t *_ipc_message_queue_init(void);
#endif