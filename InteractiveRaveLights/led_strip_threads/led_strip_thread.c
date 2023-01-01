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

/**
 * @brief Localized Module Variables
 */
static q15_t fft_data[ADC_FFT_BUFFER_SIZE];
static int values_matrix[12];
WS2812B_t *strip;
static int low_freq_divider = 190;
static int high_freq_divider = 35;

/**
 * Localized declarations
 */
static const int LOW_FREQ_LOWER_BOUNDS = 10;
static const int LOW_FREW_UPPER_BOUNDS = 450;
static const int HIGH_FREQ_LOWER_BOUNDS = 5;
static const int HIGH_FREQ_UPPER_BOUNDS = 100;
static const int HIGH_FREQ_INCREMENT = 1;
static const int LOW_FREQ_DECREMENT = 5;

void led_strip_thread_one_init(void *params)
{
    strip = setup_ws2812b_strip(96);
    update_ws2812b_strip(strip);
    memset(values_matrix, 0, sizeof(values_matrix));
}

void led_strip_thread_one(void *params)
{
    for (;;)
    {
        fft_copy_buffer(fft_data, sizeof(fft_data));

        for (int x = 0; x < 12; x++)
        {
            if (values_matrix[x] > 0)
                values_matrix[x]--;
        }

        for (int x = 0; x < 96; x++)
        {
            set_ws2812b_strip_rgb(strip, x, (rgb_color){0, 0, 0});
        }

        int num_low_clips = 0;
        int num_high_clips = 0;
        for (int k = 0; k < 2; k++)
        {
            int value = fft_data[30 + k * 8];
            value = value;
            hsv_color col;
            col.h = value / 10;
            col.v = 80;
            col.s = 255;
            value = value / low_freq_divider;

            if (value > 7)
            {
                num_high_clips++;
                value = 7;
            }
            if (value == 0)
            {
                num_low_clips++;
            }
            if (num_low_clips >= 7)
            {
                if (low_freq_divider >= LOW_FREQ_LOWER_BOUNDS)
                    low_freq_divider -= LOW_FREQ_DECREMENT;
            }
            if (num_high_clips >= 7)
            {
                low_freq_divider += LOW_FREQ_DECREMENT;
                if (low_freq_divider <= LOW_FREW_UPPER_BOUNDS)
                    low_freq_divider = LOW_FREW_UPPER_BOUNDS;
            }

            if (values_matrix[k] < value)
                values_matrix[k] = value;

            for (int y = 0; y < values_matrix[k]; y++)
            {
                set_ws2812b_strip_hsv(strip, k * 8 + y, col);
            }
        }

        num_low_clips = 0;
        num_high_clips = 0;
        for (int k = 2; k < 12; k++)
        {
            int value = fft_data[30 + k * 8];
            value = value;
            hsv_color col;
            col.h = value / (2 * high_freq_divider / 35);
            col.v = 80;

            int saturation = 555 - (value / (high_freq_divider / 35));
            if (saturation > 255)
                saturation = 255;
            // Negative clipping
            if (saturation < 0)
                saturation = 0;
            col.s = saturation;
            value = value / high_freq_divider;

            if (value > 7)
            {
                num_high_clips++;
                value = 7;
            }
            if (value == 0)
            {
                num_low_clips++;
            }
            if (num_low_clips >= 7)
            {
                if (high_freq_divider <= HIGH_FREQ_LOWER_BOUNDS)
                    high_freq_divider -= HIGH_FREQ_INCREMENT;
            }
            if (num_high_clips >= 7)
            {
                high_freq_divider += HIGH_FREQ_INCREMENT;
                if (high_freq_divider >= HIGH_FREQ_UPPER_BOUNDS)
                {
                    high_freq_divider = HIGH_FREQ_UPPER_BOUNDS;
                }
            }

            if (values_matrix[k] < value)
                values_matrix[k] = value;

            for (int y = 0; y < values_matrix[k]; y++)
            {
                set_ws2812b_strip_hsv(strip, k * 8 + y, col);
            }
        }
        update_ws2812b_strip(strip);
        usleep(10000);
    }
}