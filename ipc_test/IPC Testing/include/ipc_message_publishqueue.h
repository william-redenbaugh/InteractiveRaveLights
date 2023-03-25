#ifndef IPC_MESSAGE_PUBLISHQUEUE_H
#define IPC_MESSAGE_PUBLISHQUEUE_H
#include "ipc.h"

#define IPC_QUEUE_MAX_NUM_ELEMENTS 16
#define TASK1_BIT   (1UL << 0UL) // zero shift for bit0

typedef enum ipc_message_callback_status
{
    IPC_MESSAGE_COMPLETE_SUCCESS,
    IPC_MESSAGE_COMPLETE_FAIL,
    IPC_MESSAGE_COMPLETE_FAIL_TIMEOUT,
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
    MutexLock ipc_message_node_muttx;

    // Signal handler detecting new message
    OSSignal new_msg_cv;
    OSSignal ack_msg_mp;
    MutexLock new_msg_mp;
} ipc_message_publish_module_t;

extern ipc_message_publish_module_t *ipc_publush_queue_module;

/**
 * @brief Allows us to add a message to the message queue.
 * @note Thread safe!!!
 * @note internal call only
 * @param ipc_message_publish_module_t *module pointer to the module that we are publishing to
 * @param ipc_message_node_t message that we are pushing
 */
bool _ipc_push_message_queue(ipc_message_publish_module_t *module, ipc_message_node_t node);

/**
 * @brief Allows us to add a message to the message queue.
 * @note Thread safe!!!
 * @note internal call only
 * @param ipc_message_publish_module_t *module pointer to the module that we are publishing to
 * @param ipc_message_node_t pointer *message that we are consuming
 */
bool _ipc_pop_message_queue(ipc_message_publish_module_t *module, ipc_message_node_t *node);

/**
 * @brief Sets up our IPC message queue
 */
void init_ipc_message_queue(void);

/**
 * @brief Signals that a new event has been published to the module
 * @note internal call only!!!
 * @param ipc_message_publish_module_t *module pointer to the module that we are publishing to
 */
int _signal_new_event(ipc_message_publish_module_t *module);
/**
 * @brief Signals that a new event has ben published to the module
 */
int signal_new_event(void);

/**
 * @brief Blocks until a new event has been published
 * @note internal call only!!!
 * @param ipc_message_publish_module_t *module pointer to the module that we are publishing to
 */
void _ipc_msg_queue_wait_new_event(ipc_message_publish_module_t *module);

/**
 * @brief Blocks until a new event has been published
 */
void ipc_msg_queue_wait_new_event(void);

/**
 * @brief submits a new event to the message publish mmodule
 * @note internal call only
 * @param ipc_message_publish_module_t *module pointer to the module that we are publishing to
 * @param ipc_message_node_t pointer *message that we are consuming
 */
bool _ipc_publish_message(ipc_message_publish_module_t *module, ipc_message_node_t node);

/**
 * @brief submits a new event to the message publish mmodule
 * @param ipc_message_node_t pointer *message that we are consuming
 */
bool ipc_publish_message(ipc_message_node_t node);

/**
 * @brief Signals that we have recieved our ack from the IPC layer that message was recieved
 * @note internal call only
*/
int  _ipc_msg_ack_cmd_recv(ipc_message_publish_module_t *module);

/**
 * @brief Signals that we have recieved our ack from the IPC layer that message was recieved
*/
int ipc_msg_ack_cmd_recv(void);

/**
 * @brief Blocks until we recieve the call that our messsage ack has been recieved
 * @note internal call only
*/
bool _ipc_msg_wait_recieve_cmd_ack(ipc_message_publish_module_t *module);

/**
 * @brief Blocks until we recieve the call that our messsage ack has been recieved
*/
bool ipc_msg_wait_recieve_cmd_ack(void);

/**
 * @brief Blocks until there's an event in queue, then consumes that event
 * @param ipc_message_publish_module_t *module pointer to the module that we are publishing to
 * @note internal call only!
 */
ipc_message_node_t _ipc_block_consume_new_event(ipc_message_publish_module_t *module);

/**
 * @brief Blocks until there's an event in queue, then consumes that event
 */
ipc_message_node_t ipc_block_consume_new_event(void);

/**
 * @brief Initializes the ipc message queue to be used by all!
 */
ipc_message_publish_module_t *_ipc_message_queue_init(void);
#endif