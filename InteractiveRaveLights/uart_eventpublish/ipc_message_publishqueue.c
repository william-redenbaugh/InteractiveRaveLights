#include "ipc_message_publishqueue.h"

ipc_message_publish_module_t *ipc_publush_queue_module;

bool ipc_publish_message(ipc_message_node_t node)
{
    _ipc_publish_message(ipc_publush_queue_module, node);
}

bool _ipc_publish_message(ipc_message_publish_module_t *module, ipc_message_node_t node)
{
}

bool _ipc_push_message_queue(ipc_message_publish_module_t *module, ipc_message_node_t node)
{
    if (module->max_size == module->current_size)
        return false;
}

bool _ipc_pop_message_queue(ipc_message_publish_module_t *module, ipc_message_node_t node)
{
}

/***/
ipc_message_publish_module_t *_ipc_message_queue_init(void)
{
    ipc_message_publish_module_t *module = malloc(sizeof(ipc_message_publish_module_t));
    module->max_size = IPC_QUEUE_MAX_NUM_ELEMENTS;
    module->node_list = malloc(sizeof(ipc_message_node_t) * module->max_size);
    module->current_size = 0;
    module->current_pos = 0;
}