
#include "OS/OSThreadKernel.h"
#include <Arduino.h>
#include "ipc_message_publishqueue.h"

static ipc_message_node_t node;

char test[] = "helloworld\n";
void test_thread(void *param){
   for(;;){


        node.buffer_ptr = (uint8_t*)test;
        node.message_header.message_len = sizeof(test);
        node.message_header.message_type_enum = IPC_MESSAGE_ACK;
        node.callback_func = NULL;
        node.message_header.message_id = IPC_TYPE_ACK;

        ipc_publish_message(node);
        os_thread_delay_ms(6000);

    }
}