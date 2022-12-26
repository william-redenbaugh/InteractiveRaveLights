#include "led_matrix_runtime.h"
#include <nuttx/config.h>
#include <stdio.h>
#include "thread_init/threads_init.h"
#include "pthread.h"
#include "pubsub-c/pubsub.h"
#include "fft_runtime/fft_runtime.h"
#include "shared_constants/shared_constants.h"
#include "rgbMatrixI2CDriver/rgbMatrix.h"
#include "arm_math.h"
#include "dsp/fast_math_functions.h"
#include "arm_const_structs.h"
#include "ws2812b/ws2812b.h"
#include "ws2812b/hsv2rgb.h"

/* I2C settings */

#define I2C_SLAVE_ADDR 0x24
#define I2C_SPEED I2C_SPEED_FAST

/* ICMD defnitions */

#define ICMD_AVAILABLE_SIZE(n) (0x10 + (n))
#define ICMD_FIXLEN_TRANS(n) (0x80 + (n))
#define ICMD_VARLEN_TRANS(n) (0xa0 + (n))

/* User defined buffer id */

#define WRITE_BUFFER 0
#define READ_BUFFER 1
#define VERSION_BUFFER 2
#define TIMESTAMP_BUFFER 3

q15_t fft_data[ADC_FFT_BUFFER_SIZE];
struct I2C1735 handle;
static pthread_cond_t signal_matrix_complate;
static pthread_mutex_t signal_mt;

static void draw_bitmap_red(uint8_t image[8][8])
{
    disp_show_color(&handle, 0, 0, 0);
    for (int x = 0; x < 8; x++)
    {
        for (int y = 0; y < 8; y++)
        {
            if (image[x][y] == 1)
            {
                uint8_t pos[2] = {x, y};
                draw_point_color(&handle, pos, 120, 0, 0);
            }
        }
    }
    image_test(&handle);
}

void led_matrix_init(void *ptr){
    memset(&handle, 0, sizeof(handle));
    RGBMatrixInit(&handle);
    pthread_mutex_init(&signal_mt, NULL);
    pthread_cond_init(&signal_matrix_complate, NULL);
}

void wait_matrix_complete(void){
    pthread_mutex_lock(&signal_mt);
    pthread_cond_wait(&signal_matrix_complate, &signal_mt);
}

static int values[8];
static int values_matrix[12];
static q15_t log_fft_data[ADC_FFT_BUFFER_SIZE];
int decrement = 0;
void led_matrix_runtime(void *ptr){

    WS2812B_t *strip = setup_ws2812b_strip(96);

    update_ws2812b_strip(strip);

    for(;;){
        fft_copy_data(fft_data, sizeof(fft_data));

        decrement++;
        if(decrement == 2){
            for(int x = 0; x <  8; x++){
                if(values[x] > 0)
                    values[x]--;
            }
            decrement = 0;
        }

        for(int n = 0; n < 12; n++){
            values_matrix[n]--;
        }
        for(int n = 0; n < 96; n++){
            set_ws2812b_strip_rgb(strip, n, (rgb_color){0, 0, 0});
        }

        for(int x = 0; x < 12; x++){
            int value = fft_data[x * 8];
            int value_y = value/20;
            int hue = value/2;
            if(value_y > 7)
                value_y = 7;
            if(values_matrix[x] < value_y){
                values_matrix[x] = value_y;
            }
            hsv_color col;
            col.h = hue;
            col.s = 255;
            col.v = 100;
            for(int y = 0; y < values_matrix[x]; y++){
                set_ws2812b_strip_hsv(strip, 8 * x + y, col);
            }
        }

        update_ws2812b_strip(strip);


        for(int x = 0; x <  8; x++){
            int value = fft_data[x * 14 + 20];
            if(value < 0)
                value = 0;
            value = value / 55;
            if(value > 7)
                value = 7;

            if(values[x] < value){
                values[x] = value;
            }

            for(int y = 0; y < values[x]; y++){
                uint8_t pos[2] = {y, x};
                draw_point_color(&handle, pos, 20, 20, 20);
            }

            for(int y = values[x]; y < 8; y++){
                uint8_t pos[2] = {y, x};
                draw_point_color(&handle, pos, 0, 0, 0);
            }
        }

        image_test(&handle);
        usleep(10000);
    }
}