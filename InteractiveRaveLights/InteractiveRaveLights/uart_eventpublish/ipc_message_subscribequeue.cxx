#include "ipc_message_subscribequeue.h"
#include "ipc_message_publishqueue.h"
#include "../common_ipc_enum/common_ipc_enum.h"

ipc_subscrube_module_t *ipc_subscribe_module_main;

ipc_subscrube_module_t *new_ipc_module(void)
{
    ipc_subscrube_module_t *mod = (ipc_subscrube_module_t*)malloc(sizeof(ipc_subscrube_module_t));
    for(int n = 0; n < IPC_TYPE_ENUM_LEN; n++){
        mod->msg_sub_heads_list[n].head = NULL;
        pthread_mutex_init(&mod->msg_sub_heads_list[n].ipc_message_node_muttx, NULL);
    }
    return mod;
}

void init_ipc_module(void)
{
    ipc_subscribe_module_main = new_ipc_module();
}

bool _ipc_attach_ipc_cb(ipc_subscrube_module_t *mod, ipc_sub_cb cb, int32_t message_id){
    if(message_id >= IPC_TYPE_ENUM_LEN || message_id < 0){
        return false;
    }

    ipc_subscribe_cb_node_t *node = NULL;
    if(mod->msg_sub_heads_list[message_id].head == NULL){
        mod->msg_sub_heads_list[message_id].head = (ipc_subscribe_cb_node_t*)malloc(sizeof(ipc_subscribe_cb_node_t));
        node = mod->msg_sub_heads_list[message_id].head;
    }
    else{
        // Append to end of node
        node = mod->msg_sub_heads_list[message_id].head;
        while(node->next != NULL){
            node = node->next;
        }
    }

    node->sub_cb = cb;
}

bool ipc_attach_ipc_cb(ipc_sub_cb cb, int32_t message_id){
    _ipc_attach_ipc_cb(ipc_subscribe_module_main, cb, message_id);
}


bool _ipc_run_all_sub_cb(ipc_message_header_t header, uint8_t *data, ipc_subscrube_module_t *mod)
{
    if (header.message_id < 0 || header.message_id >= IPC_TYPE_ENUM_LEN)
    {
        return false;
    }

    // If the message we received is an ACK, then we want to let the publish module know the message has been recieved
    // If the message we received isn't an ACK, then we have to send an ACK back through the IPC layer
    if (IPC_MESSAGE_ACK != header.message_type_enum)
    {
        ipc_message_node_t message;
        message.buffer_ptr = NULL;
        message.callback_func = NULL;
        message.message_header.message_id = IPC_TYPE_ACK;
        message.message_header.message_len = 0;
        message.message_header.message_type_enum = IPC_MESSAGE_ACK;
        ipc_publish_message(message);
    }
    else{
        ipc_msg_ack_cmd_recv();
    }

    // Hash index based off the message id.
    ipc_subscribe_cb_list_t list = mod->msg_sub_heads_list[header.message_id];
    pthread_mutex_lock(&list.ipc_message_node_muttx);

    ipc_subscribe_cb_node_t *node = list.head;
    while (node != NULL)
    {
        ipc_sub_ret_cb_t ret_cb;

        // The data we are passing in
        ret_cb.data = data;

        // Header of data
        ret_cb.msg_header = header;

        if(node->sub_cb != NULL){
            // Run callback
            node->sub_cb(ret_cb);
        }
        node = node->next;
    }

    pthread_mutex_unlock(&list.ipc_message_node_muttx);
}

bool ipc_run_all_sub_cb(ipc_message_header_t header, uint8_t *data)
{
    return _ipc_run_all_sub_cb(header, data, ipc_subscribe_module_main);
}

bool _ipc_attach_cb(ipc_subscrube_module_t *mod, int message_id, ipc_sub_cb specified_cb)
{
    // Any conditons that might not allow us to attach the callback
    // message id below zero or out of bounds
    // Null pointer exception
    // Callback pointer exception
    if (mod == NULL || message_id < 0 || message_id >= IPC_TYPE_ENUM_LEN || specified_cb == NULL)
        return false;

    // Hash index based off the message id.
    ipc_subscribe_cb_list_t list = mod->msg_sub_heads_list[message_id];
    pthread_mutex_lock(&list.ipc_message_node_muttx);

    ipc_subscribe_cb_node_t *node = list.head;
    if (node == NULL)
    {
        list.head = (ipc_subscribe_cb_node_t *)malloc(sizeof(ipc_subscribe_cb_node_t));
        list.head->next = NULL;
        list.head->sub_cb = specified_cb;
    }
    else
    {
        while (node->next != NULL)
        {
            node = node->next;
        }

        node->next = (ipc_subscribe_cb_node_t *)malloc(sizeof(ipc_subscribe_cb_node_t));
        node = node->next;
        node->next = NULL;
        node->sub_cb = specified_cb;
    }

    return true;
}

bool ipc_attach_cb(int message_id, ipc_sub_cb specified_cb)
{
    return _ipc_attach_cb(ipc_subscribe_module_main, message_id, specified_cb);
}
