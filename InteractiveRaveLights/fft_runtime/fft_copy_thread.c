#include "fft_copy_thread.h"
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

static q15_t shared_buffer[ADC_FFT_BUFFER_SIZE * 2];
static pthread_mutex_t shared_buffer_mtx;
static q15_t fft_data[ADC_FFT_BUFFER_SIZE * 2];

void init_fft_copy_mod(void *params){
    pthread_mutex_init(&shared_buffer_mtx, NULL);
}

void fft_copy_buffer(q15_t *buff, size_t size){
    memcpy(buff, shared_buffer, size);
}

void fft_copy_thread(void *params){

    for(;;){
        fft_copy_data(fft_data, sizeof(fft_data));

        pthread_mutex_lock(&shared_buffer_mtx);
        memcpy(shared_buffer, fft_data, sizeof(q15_t) * ADC_FFT_BUFFER_SIZE * 2);
        pthread_mutex_unlock(&shared_buffer_mtx);

        usleep(10000);
    }
}