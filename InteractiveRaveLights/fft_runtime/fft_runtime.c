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

void fft_primary_task(void *ptr){
    printf("CMSIS init start\n");
    static arm_rfft_instance_q15 fft_instance;
    static q15_t output[ADC_FFT_BUFFER_SIZE * 2];

    arm_status status;
    status = arm_rfft_init_q15(&fft_instance, ADC_FFT_BUFFER_SIZE, 0, 1);
    uint16_t input_buffer[ADC_FFT_BUFFER_SIZE];

    printf("CMSIS fft init complete.\n");
    for(;;){
        // Copy data into our own buffer
        adc_copy_filtered_data(input_buffer, ADC_FFT_BUFFER_SIZE);

        // CMSIS FFT copy
        arm_rfft_q15(&fft_instance, (q15_t*)input_buffer, output);

        printf("CMSIS FFT completed computation\n");
        usleep(10000);
    }
}