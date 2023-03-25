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

    module->new_msg_mp.lockWaitIndefinite();
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

    module->new_msg_mp.unlock();
    return true;
}

bool _ipc_pop_message_queue(ipc_message_publish_module_t *module, ipc_message_node_t *node)
{
    module->new_msg_mp.lockWaitIndefinite();
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
    module->new_msg_mp.unlock();

    return true;
}

int _signal_new_event(ipc_message_publish_module_t *module)
{
    module->new_msg_cv.signal(THREAD_SIGNAL_0);
    return 9;
}

void _ipc_msg_queue_wait_new_event(ipc_message_publish_module_t *module)
{
    module->ipc_message_node_muttx.lockWaitIndefinite();
    int num_msg = module->current_size;
    module->ipc_message_node_muttx.unlock();

    // If there are no messages in queue
    if (num_msg == 0)
        module->new_msg_cv.wait_notimeout(THREAD_SIGNAL_0);
    module->new_msg_cv.clear(THREAD_SIGNAL_0);
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

int  _ipc_msg_ack_cmd_recv(ipc_message_publish_module_t *module){
    int ret = 0;
    module->ack_msg_mp.signal(THREAD_SIGNAL_0);

    return ret;
}

int ipc_msg_ack_cmd_recv(void){
    return _ipc_msg_ack_cmd_recv(ipc_publish_queue_module);
}

bool _ipc_msg_wait_recieve_cmd_ack(ipc_message_publish_module_t *module){
    return module->ack_msg_mp.wait_n_clear(THREAD_SIGNAL_0, 5000);
}

bool ipc_msg_wait_recieve_cmd_ack(void){
    _ipc_msg_wait_recieve_cmd_ack(ipc_publish_queue_module);
}

ipc_message_publish_module_t *_ipc_message_queue_init(void)
{

    ipc_message_publish_module_t *module =  new ipc_message_publish_module_t;
    module->max_size = IPC_QUEUE_MAX_NUM_ELEMENTS;
    module->node_list =  new ipc_message_node_t[20];
    module->current_size = 0;
    module->head_pos = 0;
    module->tail_pos = 0;

    //module->ipc_message_node_muttx = xSemaphoreCreateBinary();

    //module->new_msg_cv = xEventGroupCreate();
    //module->new_msg_mp = xSemaphoreCreateBinary();

    return module;
}

void init_ipc_message_queue(void)
{
    ipc_publish_queue_module = _ipc_message_queue_init();
}