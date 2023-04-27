#include "beat_detection.h"
#include "fft_runtime/fft_copy_thread.h"
#include <nuttx/config.h>
#include <stdio.h>
#include "thread_init/threads_init.h"
#include "pthread.h"
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

/**
 * @brief LOCAL VARIABLE DECLARAIONS
 */
static q15_t fft_data[ADC_FFT_BUFFER_SIZE];
static struct work_s beat_checks_worker;
static int num_increment = 0;

#define BEAT_DETECTION_WHICH_SAMPLE 30
#define BEAT_DETECTION_THRESHOLD 300
#define BEAT_DETECTION_INTERVAL 5000

static void beat_checks_wq(int argc, char *argv[])
{
    int ave_bpm = num_increment * 12;
    num_increment = 0;

    //printf("Average BPM %d\n", ave_bpm);
    int ret = work_queue(LPWORK, &beat_checks_worker, (worker_t)beat_checks_wq, NULL, MSEC2TICK(BEAT_DETECTION_INTERVAL));
}

/**
 * @brief Module intialization for the beat detection
 * @note Just starts the work queue that will periodically manage beat detection
 */
void beat_detection_module_init(void *params)
{
    int ret = work_queue(LPWORK, &beat_checks_worker, (worker_t)beat_checks_wq, NULL, MSEC2TICK(BEAT_DETECTION_INTERVAL));
}

/**
 * @brief Thread that will help with the algorithm that detects the rough BPM
 */
void beat_detection_thread(void *params)
{

    for (;;)
    {
        fft_copy_buffer(fft_data, sizeof(fft_data));

        if (fft_data[BEAT_DETECTION_WHICH_SAMPLE] > BEAT_DETECTION_THRESHOLD)
        {
            num_increment++;
        }

        usleep(200000);
    }
}