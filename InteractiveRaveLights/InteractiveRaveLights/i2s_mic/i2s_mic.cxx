#include "i2s_mic.h"
#include "CSAL/os_error.h"
#include <nuttx/config.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <fcntl.h>
#include <errno.h>
#include <arch/board/board.h>
#include <asmp/mpshm.h>
#include <arch/chip/pm.h>
#include <sys/stat.h>


#include "memutils/os_utils/chateau_osal.h"
#include "audio/audio_high_level_api.h"
#include "memutils/message/Message.h"
#include "msgq_id.h"
#include "msgq_pool.h"

// Shared memory handler
static mpshm_t s_shm;

// Default Mic Volume(for now)
#define DEF_VOLUME -20


static bool app_init_libraries(void)
{
  int ret;
  uint32_t addr = MSGQ_TOP_DRM;

  // Initialize shared memory

  //ret = mpshm_init(&s_shm, 1, 1024 * 128);
  if (ret < 0)
    {
      printf("Error: mpshm_init() failure. %d\n", ret);
      return false;
    }

  //ret = mpshm_remap(&s_shm, (void *)addr);
  if (ret < 0)
    {
      printf("Error: mpshm_remap() failure. %d\n", ret);
      return false;
    }

  // Initalize MessageLib.
  //err_t err = MsgLib::initFirst(NUM_MSGQ_POOLS, MSGQ_TOP_DRM);
  
  return true;
}

void i2s_mic_init(void *params){
  if(!app_init_libraries()){
    printf("Error Initializing the I2S Microphones\n");
    return;
  }
}

void i2s_mic_thread(void *params){
    app_init_libraries();
    for(;;){

        usleep(1000000);
    }
}