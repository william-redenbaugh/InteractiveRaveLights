#include <nuttx/config.h>
#include <stdio.h>
#include "thread_init/threads_init.h"
#include "pthread.h"
#include "pubsub-c/pubsub.h"
#include "max7219_led_driver/max7219_led_driver.h"

int main(int argc, char *argv[])
{
  ps_init();
  threads_list_init();
  while(true)
  {
    usleep(10000000);
  }
  return 0;
}
