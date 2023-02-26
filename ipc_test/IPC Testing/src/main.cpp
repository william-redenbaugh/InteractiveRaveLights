#include "OS/OSThreadKernel.h"
#include <Arduino.h>
#include "threads_init.h"
#include "ipc_message_publishqueue.h"

/// 4086459573
// Man Myth Legend Vinh


void setup(){
  os_init();
  threads_list_init();
}

void loop(){
  os_suspend_thread(os_current_id());
  _os_yield();
  //os_thread_delay_ms(1000);
}
