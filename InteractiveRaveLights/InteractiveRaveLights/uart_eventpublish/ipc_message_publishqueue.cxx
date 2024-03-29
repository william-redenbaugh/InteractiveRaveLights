#include "ipc_message_publishqueue.h"
#include "stdlib.h"

ipc_message_publish_module_t *ipc_publish_queue_module;

bool ipc_publish_message(ipc_message_node_t node)
{
    _ipc_publish_message(ipc_publish_queue_module, node);
}

bool _ipc_publish_message(ipc_message_publish_module_t *module, ipc_message_node_t node)
{
    // Pushes a new event.
    _ipc_push_message_queue(module, node);
    _signal_new_event(module);
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
    int ret = 0;
    
    pthread_mutex_lock(&module->ipc_message_node_muttx);
    if(module->waiting){
        module->waiting = false;
       ret = sem_post(&module->new_msg_mp);
    }
    pthread_mutex_unlock(&module->ipc_message_node_muttx);
    return ret;
}

void _ipc_msg_queue_wait_new_event(ipc_message_publish_module_t *module)
{
    pthread_mutex_lock(&module->ipc_message_node_muttx);
    int num_msg = module->current_size;
    if(num_msg == 0)
        module->waiting = true;
    pthread_mutex_unlock(&module->ipc_message_node_muttx);

    while(module->waiting){
        int ret = sem_wait(&module->new_msg_mp);
    }
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
    if(module->ack_waiting){
        module->waiting = false;
        ret = sem_post(&module->ack_msg_mp);
    }
    return ret;
}

int ipc_msg_ack_cmd_recv(void){
    _ipc_msg_ack_cmd_recv(ipc_publish_queue_module);
}

int _ipc_msg_wait_recieve_cmd_ack(ipc_message_publish_module_t *module){
    int ret = 0;
    if(module->ack_waiting){
        struct timespec timeout; 
        timeout.tv_nsec = 0; 
        timeout.tv_sec = 5;
        ret = sem_timedwait(&module->ack_msg_mp, &timeout);
    }

    return ret;
}

int ipc_msg_wait_recieve_cmd_ack(void){
    _ipc_msg_wait_recieve_cmd_ack(ipc_publish_queue_module);
}

ipc_message_publish_module_t *_ipc_message_queue_init(void)
{
    ipc_message_publish_module_t *module = (ipc_message_publish_module_t*)malloc(sizeof(ipc_message_publish_module_t));

    module->max_size = IPC_QUEUE_MAX_NUM_ELEMENTS;
    module->node_list = (ipc_message_node_t*)malloc(sizeof(ipc_message_node_t) * module->max_size);
    module->current_size = 0;
    module->head_pos = 0;
    module->tail_pos = 0;
    module->waiting = false;
    module->ack_waiting = false;

    // Initialize synchro mutex
    pthread_mutex_init(&module->ipc_message_node_muttx, NULL);

    // Initialize our flag to send messages to queue
    sem_init(&module->new_msg_mp, 0, 0);
    sem_setprotocol(&module->new_msg_mp, SEM_PRIO_NONE);

    // Initialize our IPC ack flag
    sem_init(&module->ack_msg_mp, 0, 0);
    sem_setprotocol(&module->ack_msg_mp, SEM_PRIO_NONE);

    return module;
}

void init_ipc_message_queue(void)
{
    ipc_publish_queue_module = _ipc_message_queue_init();
}