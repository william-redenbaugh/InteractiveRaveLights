#include "led_matrix_runtime.h"
#include <nuttx/config.h>
#include <stdio.h>
#include "thread_init/threads_init.h"
#include "pthread.h"
#include "pubsub-c/pubsub.h"
#include "fft_runtime/fft_copy_thread.h"
#include "shared_constants/shared_constants.h"
#include "rgbMatrixI2CDriver/rgbMatrix.h"
#include "arm_math.h"
#include "dsp/fast_math_functions.h"
#include "arm_const_structs.h"
#include "ws2812b/ws2812b.h"
#include "ws2812b/hsv2rgb.h"

/**
 * @brief LOCAL VARIABLE DECLARAIONS
 */
static q15_t fft_data[ADC_FFT_BUFFER_SIZE];
struct I2C1735 handle;

static int values[8];
int decrement = 0;

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

void led_matrix_init(void *ptr)
{
    memset(&handle, 0, sizeof(handle));
    // RGBMatrixInit(&handle);
}

void led_matrix_runtime(void *ptr)
{
    memset(values, 0, sizeof(values));
    for (;;)
    {

        fft_copy_buffer(fft_data, sizeof(fft_data));
        decrement++;
        if (decrement == 2)
        {
            for (int x = 0; x < 8; x++)
            {
                if (values[x] > 0)
                    values[x]--;
            }
            decrement = 0;
        }

        for (int x = 0; x < 8; x++)
        {
            int value = fft_data[x * 16 + 20];
            if (value < 0)
                value = 0;
            value = value / 10;
            if (value > 7)
                value = 7;

            if (values[x] < value)
            {
                values[x] = value;
            }

            hsv_color hsv_col;
            hsv_col.h = value * 15;
            hsv_col.s = 255;
            hsv_col.v = 255;

            rgb_color rgb_col = hsv2rgb(hsv_col);
            for (int y = 0; y < values[x]; y++)
            {
                uint8_t pos[2] = {y, x};
                draw_point_color(&handle, pos, rgb_col.r, rgb_col.g, rgb_col.b);
            }

            for (int y = values[x]; y < 8; y++)
            {
                uint8_t pos[2] = {y, x};
                draw_point_color(&handle, pos, 0, 0, 0);
            }
        }

        // image_test(&handle);
        usleep(10000);
    }
}