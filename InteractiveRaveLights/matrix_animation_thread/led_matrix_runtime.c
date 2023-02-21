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

void wait_matrix_complete(void)
{
}

void led_matrix_runtime(void *ptr)
{

    for (;;)
    {
        fft_copy_data(fft_data, sizeof(fft_data));

        fft_copy_buffer(fft_data, sizeof(fft_data));

        // image_test(&handle);
        usleep(10000);
    }
}