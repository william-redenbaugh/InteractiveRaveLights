#include "test.h"

#include "pthread.h"
#include "uart_eventpublish/ipc_message_publishqueue.h"

void test_thread_init(void *params){

}

void test_thread(void *params){
    for(;;){
        
        ipc_message_node_t node; 
        node.message_header.message_type_enum = IPC_MESSAGE_ACK;
        node.message_header.message_id;
        ipc_publish_message(node);
        usleep(100000);
    }
}