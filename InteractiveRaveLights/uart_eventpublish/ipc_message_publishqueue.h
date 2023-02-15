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

typedef void (*ipc_message_complete_callback_t)(ipc_message_ret_t);

typedef struct ipc_message_node
{
    ipc_message_header_t message_header;
    uint8_t *buffer_ptr;
    ipc_message_complete_callback_t callback_func;
} ipc_message_node_t;

typedef struct ipc_message_publish_module
{
    ipc_message_node_t *node_list;
    int max_size;
    int current_size;
    int head_pos;
    int tail_pos;
    pthread_mutex_t ipc_message_node_muttx;
} ipc_message_publish_module_t;

extern ipc_message_publish_module_t *ipc_publush_queue_module;

/**
 * @brief Allows us to add a message to the message queue.
 * @note Thread safe!!!
 * @param ipc_message_publish_module_t *module pointer to the module that we are publishing to
 * @param ipc_message_node_t message that we are pushing
 */
bool _ipc_push_message_queue(ipc_message_publish_module_t *module, ipc_message_node_t node);

/**
 * @brief Allows us to add a message to the message queue.
 * @note Thread safe!!!
 * @param ipc_message_publish_module_t *module pointer to the module that we are publishing to
 * @param ipc_message_node_t pointer *message that we are consuming
 */
bool _ipc_pop_message_queue(ipc_message_publish_module_t *module, ipc_message_node_t *node);

bool _ipc_publish_message(ipc_message_publish_module_t *module, ipc_message_node_t node);
bool ipc_publish_message(ipc_message_node_t node);

/**
 *
 */
ipc_message_publish_module_t *_ipc_message_queue_init(void);
#endif