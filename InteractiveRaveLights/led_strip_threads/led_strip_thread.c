#include "led_strip_thread.h"
#include <nuttx/config.h>
#include <stdio.h>
#include "thread_init/threads_init.h"
#include "pthread.h"
#include "pubsub-c/pubsub.h"
#include "fft_runtime/fft_copy_thread.h"
#include "shared_constants/shared_constants.h"
#include "arm_math.h"
#include "dsp/fast_math_functions.h"
#include "arm_const_structs.h"
#include "ws2812b/ws2812b.h"
#include "ws2812b/hsv2rgb.h"

static q15_t fft_data[ADC_FFT_BUFFER_SIZE];
static int values_matrix[12];
WS2812B_t *strip;

void led_strip_thread_one_init(void *params){
    strip = setup_ws2812b_strip(96);
    update_ws2812b_strip(strip);
    memset(values_matrix, 0, sizeof(values_matrix));

}

void led_strip_thread_one(void *params){

    for(;;){
        fft_copy_buffer(fft_data,  sizeof(fft_data));

        for(int x = 0; x <  12; x++){
            if(values_matrix[x] > 0)
                values_matrix[x]--;
        }

        for(int x = 0; x < 96; x++){
            set_ws2812b_strip_rgb(strip, x, (rgb_color){0, 0, 0});
        }
        for(int k = 0; k < 12; k++){
            int value = fft_data[30 + k * 8];
            value = value;
            hsv_color col;
            col.h = value/2;
            col.v = 80;
            col.s = 255;

            value = value/30;

            if(value > 7)
                value = 7;

            if(values_matrix[k] < value)
                values_matrix[k] = value;

            for(int y = 0; y < values_matrix[k]; y++){
                set_ws2812b_strip_hsv(strip, k * 8 + y, col);
            }
        }

        update_ws2812b_strip(strip);

        usleep(10000);
    }
}