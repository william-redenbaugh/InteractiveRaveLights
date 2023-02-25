#include "ipc_message_subscribequeue.h"
#include "ipc_message_publishqueue.h"
#include "common_ipc_enum.h"

ipc_subscrube_module_t *ipc_subscribe_module_main;

ipc_subscrube_module_t *new_ipc_module(void)
{
    ipc_subscrube_module_t *mod = (ipc_subscrube_module_t*)malloc(sizeof(ipc_subscrube_module_t));
    //for(int n = 0; n < IPC_TYPE_ENUM_LEN; n++){
    //    mod->msg_sub_heads_list[n].ipc_message_node_muttx = xSemaphoreCreateBinary();
    //}
    return mod;
}

void init_ipc_module(void)
{
    ipc_subscribe_module_main = new_ipc_module();
}

bool _ipc_run_all_sub_cb(ipc_message_header_t header, uint8_t *data, ipc_subscrube_module_t *mod)
{
    if (header.message_id < 0)
    {
        return false;
    }
    // No need to send an ack if the message we just received isn't an ack
    if (IPC_TYPE_ACK == header.message_id)
    {
        // If we just recieved any other message though
        // We need to send an ACK back
        ipc_message_node_t message;
        message.buffer_ptr = NULL;
        message.callback_func = NULL;
        message.message_header.message_id = IPC_TYPE_ACK;
        message.message_header.message_len = 0;
        message.message_header.message_type_enum = IPC_MESSAGE_ACK;
        ipc_publish_message(message);
    }

    // Hash index based off the message id.
    ipc_subscribe_cb_list_t list = mod->msg_sub_heads_list[header.message_id];
    
    list.ipc_message_node_muttx.lockWaitIndefinite();

    ipc_subscribe_cb_node_t *node = list.head;
    while (node != NULL)
    {
        ipc_sub_ret_cb_t ret_cb;

        // The data we are passing in
        ret_cb.data = data;

        // Header of data
        ret_cb.msg_header = header;
        // Run callback
        node->sub_cb(ret_cb);
        node = node->next;
    }

    list.ipc_message_node_muttx.unlock();
    return true;
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
    list.ipc_message_node_muttx.lockWaitIndefinite();

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

    list.ipc_message_node_muttx.unlock();
    return true;
}

bool ipc_attach_cb(int message_id, ipc_sub_cb specified_cb)
{
    return _ipc_attach_cb(ipc_subscribe_module_main, message_id, specified_cb);
}