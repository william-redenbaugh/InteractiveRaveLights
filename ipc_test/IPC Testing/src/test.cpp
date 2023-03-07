
#include "OS/OSThreadKernel.h"
#include <Arduino.h>
#include "threads_init.h"
#include "ipc_message_publishqueue.h"

void test_thread(void *param){
    Serial.begin(115200);
   for(;;){
        /*
        ipc_message_node_t node;
        node.buffer_ptr = NULL;
        node.message_header.message_len = 0;
        node.message_header.message_type_enum = IPC_MESSAGE_ACK;
        node.callback_func = NULL;
        node.message_header.message_id = IPC_TYPE_ACK;

        ipc_publish_message(node);
        os_thread_delay_ms(1000);In
        */

       Serial.println("hello worldasfdasdf");
       os_thread_delay_s(1);
    }
}