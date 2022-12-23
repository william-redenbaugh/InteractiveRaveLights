#include <nuttx/config.h>
#include <stdio.h>
#include "thread_init/threads_init.h"
#include "pthread.h"
#include "pubsub-c/pubsub.h"
#include "max7219_led_driver/max7219_led_driver.h"
#include "Sony-Spressense-I2C-LCD-Display-Library/liquidcrystal-i2c.h"

led_matrix_t matrix;
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