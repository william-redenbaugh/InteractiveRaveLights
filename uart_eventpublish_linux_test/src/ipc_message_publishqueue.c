#include "ipc_message_publishqueue.h"

ipc_message_publish_module_t *ipc_publish_queue_module;

bool ipc_publish_message(ipc_message_node_t node)
{
    _ipc_publish_message(ipc_publish_queue_module, node);
}

bool _ipc_publish_message(ipc_message_publish_module_t *module, ipc_message_node_t node)
{
    // Pushes a new event.
    _signal_new_event(module);
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

int _signal_new_event(ipc_message_publish_module_t *module)
{
    int ret = pthread_cond_signal(&module->new_msg_cv);
    return ret;
}

void _ipc_msg_queue_wait_new_event(ipc_message_publish_module_t *module)
{
    pthread_mutex_lock(&module->ipc_message_node_muttx);
    int num_msg = module->current_size;
    pthread_mutex_unlock(&module->ipc_message_node_muttx);

    // If there are no messages in queue
    if (num_msg == 0)
        pthread_cond_wait(&module->new_msg_cv, &module->new_msg_mp);
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
    ipc_message_publish_module_t *module = malloc(sizeof(ipc_message_publish_module_t));
    module->max_size = IPC_QUEUE_MAX_NUM_ELEMENTS;
    module->node_list = malloc(sizeof(ipc_message_node_t) * module->max_size);
    module->current_size = 0;
    module->head_pos = 0;
    module->tail_pos = 0;

    // Initialize synchro mutex
    pthread_mutex_init(&module->ipc_message_node_muttx, NULL);

    // Init signal/mutx
    pthread_mutex_init(&module->new_msg_mp, NULL);
    pthread_cond_init(&module->new_msg_cv, NULL);
}

void init_ipc_message_queue(void)
{
    ipc_publish_queue_module = _ipc_message_queue_init();
}