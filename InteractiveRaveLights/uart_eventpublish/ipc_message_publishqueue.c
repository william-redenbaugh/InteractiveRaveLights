#include "ipc_message_publishqueue.h"

ipc_message_publish_module_t *ipc_publush_queue_module;

bool ipc_publish_message(ipc_message_node_t node)
{
    _ipc_publish_message(ipc_publush_queue_module, node);
}

bool _ipc_publish_message(ipc_message_publish_module_t *module, ipc_message_node_t node)
{
    if (module == NULL)
        module = _ipc_message_queue_init();

    _ipc_push_message_queue(module, node);
}

bool _ipc_push_message_queue(ipc_message_publish_module_t *module, ipc_message_node_t node)
{
    pthread_mutex_lock(&module->ipc_message_node_muttx);
    if (module->max_size == module->current_size)
        return false;

    // Populate index of data
    module->node_list[module->head_pos] = node;

    // Increment positions and current size
    module->head_pos++;
    module->current_size++;

    // Rotate around the circular buffer
    if (module->head_pos == module->max_size)
        module->head_pos = 0;
    pthread_mutex_unlock(&module->ipc_message_node_muttx);
}

bool _ipc_pop_message_queue(ipc_message_publish_module_t *module, ipc_message_node_t *node)
{
    pthread_mutex_lock(&module->ipc_message_node_muttx);
    if (module->current_size == 0)
        return false;

    // Populate tail pointer
    *node = module->node_list[module->tail_pos];

    // We popped off a node, so our current size goes down
    module->current_size--;

    // Increment our tail pointer
    module->tail_pos++;

    // Rotate around the circular buffer
    if (module->tail_pos == module->max_size)
        module->tail_pos = 0;
    pthread_mutex_unlock(&module->ipc_message_node_muttx);
}

/***/
ipc_message_publish_module_t *_ipc_message_queue_init(void)
{
    ipc_message_publish_module_t *module = malloc(sizeof(ipc_message_publish_module_t));
    module->max_size = IPC_QUEUE_MAX_NUM_ELEMENTS;
    module->node_list = malloc(sizeof(ipc_message_node_t) * module->max_size);
    module->current_size = 0;
    module->head_pos = 0;
    module->tail_pos = 0;
}