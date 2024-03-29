#include "ipc_message_publishqueue.h"

ipc_message_publish_module_t *ipc_publish_queue_module;

bool ipc_publish_message(ipc_message_node_t node)
{
    return _ipc_publish_message(ipc_publish_queue_module, node);
}

bool _ipc_publish_message(ipc_message_publish_module_t *module, ipc_message_node_t node)
{
    // Pushes a new event.
    _signal_new_event(module);
    return _ipc_push_message_queue(module, node);
}

bool _ipc_push_message_queue(ipc_message_publish_module_t *module, ipc_message_node_t node)
{
    xSemaphoreTake(module->new_msg_mp, portMAX_DELAY);
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
    xSemaphoreGive(module->new_msg_mp);
    
    return true;
}

bool _ipc_pop_message_queue(ipc_message_publish_module_t *module, ipc_message_node_t *node)
{
    xSemaphoreTake(module->new_msg_mp, portMAX_DELAY);
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
    xSemaphoreGive(module->new_msg_mp);

    return true;
}

int _signal_new_event(ipc_message_publish_module_t *module)
{
    int ret = xEventGroupSetBits(module->new_msg_cv, TASK1_BIT);
    return ret;
}

void _ipc_msg_queue_wait_new_event(ipc_message_publish_module_t *module)
{
    xSemaphoreTake(module->ipc_message_node_muttx, portMAX_DELAY);
    int num_msg = module->current_size;
    xSemaphoreGive(module->ipc_message_node_muttx);

    // If there are no messages in queue
    if (num_msg == 0)
        xEventGroupWaitBits(module->new_msg_cv, TASK1_BIT, 0, 0, portMAX_DELAY);
}

ipc_message_node_t _ipc_block_consume_new_event(ipc_message_publish_module_t *module)
{
    _ipc_msg_queue_wait_new_event(module);
    ipc_message_node_t node;
    _ipc_pop_message_queue(module, &node);
    return node;
}

ipc_message_node_t ipc_block_consume_new_event(void)
{
    return _ipc_block_consume_new_event(ipc_publish_queue_module);
}

int signal_new_event(void)
{
    return _signal_new_event(ipc_publish_queue_module);
}

void ipc_msg_queue_wait_new_event(void)
{
    _ipc_msg_queue_wait_new_event(ipc_publish_queue_module);
}

/***/
ipc_message_publish_module_t *_ipc_message_queue_init(void)
{
    
    ipc_message_publish_module_t *module = (ipc_message_publish_module_t*)pvPortMalloc(sizeof(ipc_message_publish_module_t));
    module->max_size = IPC_QUEUE_MAX_NUM_ELEMENTS;
    module->node_list = (ipc_message_node_t*)pvPortMalloc(sizeof(ipc_message_node_t) * module->max_size);
    module->current_size = 0;
    module->head_pos = 0;
    module->tail_pos = 0;

    module->ipc_message_node_muttx = xSemaphoreCreateBinary();

    module->new_msg_cv = xEventGroupCreate();
    module->new_msg_mp = xSemaphoreCreateBinary();

    return module;
}

void init_ipc_message_queue(void)
{
    ipc_publish_queue_module = _ipc_message_queue_init();
}