
#include "OS/OSThreadKernel.h"
#include <Arduino.h>
#include "MODULES/TeensyOSIPC/ipc_message_publishqueue.h"

static ipc_message_node_t node;

char test[] = "hellojkdkjjkj;lkj;lkjq'wepori[qpwoei[qwpoeir;lkj;asdf]];aslkdj;alslkj;lkj;lkj;lkjdksdfkjkj;lkj;alskda;lsdklaksjdf;alskdj;awer[qpwqkljas;ldkfja;sldkja;lskdfj;alksdjf;alskdjf;alpowierpqowieuqweasdnfb,amsndfa';lk';lk';lk';lk';lka's;dfka's;dlkewrqwer/.,moeir[pasoAJS;dlkjsaD;FLKAJS;OEIRUworld\n";
void test_thread(void *param){
   for(;;){
        node.buffer_ptr = (uint8_t*)test;
        node.message_header.message_len = sizeof(test);
        node.message_header.message_type_enum = IPCM_MESSAGE_NONE;
        node.callback_func = NULL;
        node.message_header.message_id = IPC_TYPE_ACK;

        ipc_publish_message(node);
        os_thread_delay_ms(1000);
    }
}