#include "test.h"

#include "pthread.h"
#include "uart_eventpublish/ipc_message_publishqueue.h"

void test_thread_init(void *params){

}

void test_thread(void *params){
    for(;;){

        ipc_message_node_t node;
        node.message_header.message_type_enum = IPCM_MESSAGE_NONE;
        node.message_header.message_id = IPC_TYPE_TEST;
        ipc_publish_message(node);
        usleep(1000000);
    }
}