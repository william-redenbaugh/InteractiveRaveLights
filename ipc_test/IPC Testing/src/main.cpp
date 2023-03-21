#include <Arduino.h>
#include "OS/OSThreadKernel.h"
#include "ipc_message_publishqueue.h"
#include "STATEMACHINE/statemachine.h"
#include "UNIT_TESTS/OS_UNIT_TESTS/os_unit_test.h"
#include "THREAD_INIT/threads_init.h"
/// 4086459573
// Man Myth Legend Vinh


void setup(){
  os_init();
  //s_init_testrun();
  threads_list_init();
}

void loop(){
  os_suspend_thread(os_current_id());
  _os_yield();
  //os_thread_delay_ms(1000);
}