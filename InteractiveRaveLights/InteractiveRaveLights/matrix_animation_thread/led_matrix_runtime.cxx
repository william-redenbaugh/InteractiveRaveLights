#include "led_matrix_runtime.h"
#include <nuttx/config.h>
#include <stdio.h>
#include "thread_init/threads_init.h"
#include "pthread.h"
#include "fft_runtime/fft_copy_thread.h"
#include "shared_constants/shared_constants.h"
#include "arm_math.h"
#include "dsp/fast_math_functions.h"
#include "arm_const_structs.h"
#include "ws2812b/ws2812b.h"
#include "ws2812b/hsv2rgb.h"

/**
 * @brief LOCAL VARIABLE DECLARAIONS
 */
static q15_t fft_data[ADC_FFT_BUFFER_SIZE];

void led_matrix_init(void *ptr)
{
}

void wait_matrix_complete(void)
{
}

void led_matrix_runtime(void *ptr)
{

    for (;;)
    {
        // image_test(&handle);
        usleep(10000);
    }
}