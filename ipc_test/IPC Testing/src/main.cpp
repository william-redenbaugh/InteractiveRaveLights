#include "OS/OSThreadKernel.h"
#include <Arduino.h>
#include "threads_init.h"
#include "ipc_message_publishqueue.h"

void setup(){
  threads_list_init();  
  os_init();
}

void loop(){
  os_suspend_thread(os_current_id());
  _os_yield();
  //os_thread_delay_ms(1000);
}
 