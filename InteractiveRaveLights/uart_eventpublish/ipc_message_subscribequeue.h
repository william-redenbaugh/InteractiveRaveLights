#ifndef _IPC_MESSAGE_SUBSCRIBEQUEUE_H
#define _IPC_MESSAGE_SUBSCRIBEQUEUE_H

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sched.h>
#include <errno.h>
#include "ipc.h"

/**
 * Queue of IPC messages
*/
typedef struct ipc_subscribe_queue{
    pthread_mutex_t ipc_message_node_muttx;
    struct ipc_subscribe_queue *next;
} ipc_subscribe_queue_t;

/**
 * @brief When a callback is registered somewhere, this is the
 * return function given to it.
*/
typedef struct ipc_sub_ret_cb{
    ipc_message_header_t msg_header;
} ipc_sub_ret_cb_t;

typedef void (*ipc_sub_cb)(ipc_sub_ret_cb_t);

#endif