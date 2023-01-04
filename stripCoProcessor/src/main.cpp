#include <Arduino.h>
#include <FreeRTOS.h>
#include "task.h"
#include "threads_init.h"
void setup() {
  Serial.begin();
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