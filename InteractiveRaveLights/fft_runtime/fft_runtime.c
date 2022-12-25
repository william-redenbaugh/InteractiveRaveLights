#include "fft_runtime.h"
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

#include "arm_math.h"
#include "arm_const_structs.h"
#include "adc_runtime/adc_runtime_main.h"
#include "shared_constants/shared_constants.h"

static arm_rfft_instance_q15 fft_instance;
static q15_t output[ADC_FFT_BUFFER_SIZE * 2];
static arm_status status;
static uint16_t input_buffer[ADC_FFT_BUFFER_SIZE];
static pthread_cond_t signal_fft_complete;
static pthread_mutex_t signal_mt;
static pthread_mutex_t fft_mt;

/**
 * @brief When
*/
void wait_new_mic_audio_data(void){
    block_until_new_adc_data();
}

void fft_module_init(void){
    printf("CMSIS init start\n");
    status = arm_rfft_init_q15(&fft_instance, ADC_FFT_BUFFER_SIZE, 0, 1);
    printf("CMSIS fft init complete.\n");

    pthread_mutex_init(&fft_mt, NULL);
    pthread_mutex_init(&signal_mt, NULL);
    pthread_cond_init(&signal_fft_complete, NULL);
}

/**
 * @brief Primary task thread handling most FFT operations
 *
*/
void fft_primary_task(void *ptr){
    for(;;){

        // wait_new_mic_audio_data();

        // Copy data into our own buffer
        adc_copy_filtered_data(input_buffer, ADC_FFT_BUFFER_SIZE);

        // CMSIS FFT copy
        pthread_mutex_lock(&fft_mt);
        arm_rfft_q15(&fft_instance, (q15_t*)input_buffer, output);
        pthread_mutex_unlock(&fft_mt);

        usleep(5000);
    }
}

void block_until_new_fft_data(void){
    pthread_mutex_lock(&signal_mt);
    pthread_cond_wait(&signal_fft_complete, &signal_mt);
    pthread_mutex_unlock(&signal_mt);
}

void fft_copy_data(q15_t *data_ptr, size_t size){
    pthread_mutex_lock(&fft_mt);
    memcpy(data_ptr, output, size);
    pthread_mutex_unlock(&fft_mt);
}