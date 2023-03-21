#include <nuttx/config.h>
#include <stdio.h>
#include "thread_init/threads_init.h"
#include "pthread.h"

int main(int argc, char *argv[])
{
  threads_list_init();
  while (true)
  {
    usleep(10000000);
  }
  return 0;
}