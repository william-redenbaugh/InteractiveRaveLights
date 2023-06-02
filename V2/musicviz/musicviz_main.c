#include <nuttx/config.h>
#include <stdio.h>
#include "CSAL_SHARED/threads_init.h"

int main(int argc, char *argv[])
{
  threads_list_init();
  return 0;
}
