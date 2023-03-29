
#include "OS/OSThreadKernel.h"
#include <Arduino.h>
#include "MODULES/TeensyOSIPC/ipc_message_publishqueue.h"
#include "LiquidCrystal_I2C.h"
#include "MODULES/TeensyOSIPC/ipc_message_subscribequeue.h"

LiquidCrystal_I2C lcd(0x27, 16, 2);


static ipc_message_node_t node;

static void update_text(ipc_sub_ret_cb_t ret){

    lcd.clear();
    // Turn on the blacklight and print a message.
    lcd.backlight();

    lcd.printf("%d Data received!\n", ret.msg_header.message_len);
}

char test[] = "hellojkdkjjkj;lkj;lkjq'wepori[qpwoei[qwpoeir;lkj;asdf]];aslkdj;alslkj;lkj;lkj;lkjdksdfkjkj;lkj;alskda;lsdklaksjdf;alskdj;awer[qpwqkljas;ldkfja;sldkja;lskdfj;alksdjf;alskdjf;alpowierpqowieuqweasdnfb,amsndfa';lk';lk';lk';lk';lka's;dfka's;dlkewrqwer/.,moeir[pasoAJS;dlkjsaD;FLKAJS;OEIRUworld\n";
void test_thread(void *param){
    // initialize the LCD
	lcd.begin();

   ipc_attach_cb(update_text, IPC_TYPE_TEST);
   for(;;){
        node.buffer_ptr = (uint8_t*)test;
        node.message_header.message_len = sizeof(test);
        node.message_header.message_type_enum = IPCM_MESSAGE_NONE;
        node.callback_func = NULL;
        node.message_header.message_id = IPC_TYPE_LED_ANIMATION_TYPE;

        ipc_publish_message(node);
        os_thread_delay_ms(1000);
    }
}