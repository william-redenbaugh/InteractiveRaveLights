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
 * @brief When a callback is registered somewhere, this is the
 * return function given to it.
 */
typedef struct ipc_sub_ret_cb
{
    ipc_message_header_t msg_header;
    uint8_t *data;
} ipc_sub_ret_cb_t;

typedef void (*ipc_sub_cb)(ipc_sub_ret_cb_t);
typedef struct ipc_subscribe_cb_node
{
    ipc_sub_cb sub_cb;
    struct ipc_subscribe_cb_node *next;
} ipc_subscribe_cb_node_t;

typedef struct ipc_subscribe_cb_list
{
    ipc_subscribe_cb_node_t *head;
    pthread_mutex_t ipc_message_node_muttx;
} ipc_subscribe_cb_list_t;

typedef struct ipc_subscrube_module
{
    ipc_subscribe_cb_list_t msg_sub_heads_list[IPC_TYPE_ENUM_LEN];
} ipc_subscrube_module_t;

extern ipc_subscrube_module_t *ipc_subscribe_module_main;

bool _ipc_run_all_sub_cb(ipc_message_header_t header, uint8_t *data, ipc_subscrube_module_t *mod);
bool ipc_run_all_sub_cb(ipc_message_header_t header, uint8_t *data);
bool _ipc_attach_cb(ipc_subscrube_module_t *mod, int message_id, ipc_sub_cb specified_cb);
bool _ipc_attach_ipc_cb(ipc_subscrube_module_t *mod, ipc_sub_cb cb, int32_t message_id);
bool ipc_attach_ipc_cb(ipc_sub_cb cb, int32_t message_id);

void init_ipc_module(void);
#endif