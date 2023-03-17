
#include "OS/OSThreadKernel.h"
#include <Arduino.h>
#include "ipc_message_publishqueue.h"

void test_thread(void *param){
    Serial3.begin(115200);
    pinMode(LED_BUILTIN, OUTPUT);

    for(int n = 0; n < 30;n++){
            digitalWrite(LED_BUILTIN, HIGH);
            os_thread_delay_ms(20);
            digitalWrite(LED_BUILTIN, LOW);
            os_thread_delay_ms(20);
            //Serial.println();
        }
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


        String str = Serial.readString();

        for(int n = 0; n < str.length();n++){
            digitalWrite(LED_BUILTIN, HIGH);
            os_thread_delay_ms(20);
            digitalWrite(LED_BUILTIN, LOW);
            os_thread_delay_ms(20);
            //Serial.println();
        }
    }
}