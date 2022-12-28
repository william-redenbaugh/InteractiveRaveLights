#include "beat_detection.h"
#include "fft_runtime/fft_copy_thread.h"
#include <nuttx/config.h>
#include <stdio.h>
#include "thread_init/threads_init.h"
#include "pthread.h"
#include "pubsub-c/pubsub.h"
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include "pthread.h"
#include "nuttx/wqueue.h"

#include "arm_math.h"
#include "arm_const_structs.h"
#include "adc_runtime/adc_runtime_main.h"
#include "shared_constants/shared_constants.h"

static q15_t fft_data[ADC_FFT_BUFFER_SIZE];
static struct work_s beat_checks_worker;
static uint16_t interval = 5000;

static void beat_checks_wq(int argc, char *argv[]){
    printf("hello world\n");
    int ret = work_queue(LPWORK, &beat_checks_worker, (worker_t)beat_checks_wq, NULL, MSEC2TICK(interval));
}

void beat_detection_module_init(void *params){
    printf("init module\n");
    //int ret = work_queue(LPWORK, &beat_checks_worker, (worker_t)beat_checks_wq, NULL, MSEC2TICK(interval));
}

void beat_detection_thread(void *params){

    for(;;){
        fft_copy_buffer(fft_data, sizeof(fft_data));

        usleep(10000);
    }
}