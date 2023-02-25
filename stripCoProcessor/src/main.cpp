#include <Arduino.h>
#include <FreeRTOS.h>
#include "task.h"
#include "threads_init.h"
#include "ipc_message_publishqueue.h"
#include "threads_list.h"
#include "ipc_thread.h"
void setup() {
  threads_list_init();
}

void loop() {
  vTaskSuspend(NULL);
}

void setup1(){
}

void loop1(){
  vTaskSuspend(NULL);
}