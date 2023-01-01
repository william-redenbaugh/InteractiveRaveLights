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
 * Localized declarations
 */
static const int LOW_FREQ_LOWER_BOUNDS = 10;
static const int LOW_FREW_UPPER_BOUNDS = 450;
static const int HIGH_FREQ_LOWER_BOUNDS = 5;
static const int HIGH_FREQ_UPPER_BOUNDS = 100;
static const int HIGH_FREQ_INCREMENT = 1;
static const int LOW_FREQ_DECREMENT = 5;

/**
 * @brief Localized Module Variables
 */

typedef struct led_strip_thread_processing
{
    q15_t fft_data[ADC_FFT_BUFFER_SIZE];
    int values_matrix[12];
    WS2812B_t *strip;
    int low_freq_divider;
    int high_freq_divider;
} led_strip_thread_processing_t;

static led_strip_thread_processing_t *strip_processing_thread;

void led_strip_thread_one_init(void *params)
{
    // Allocate space for our thread processing module
    strip_processing_thread = malloc(sizeof(led_strip_thread_processing_t));

    // Initialize strip
    strip_processing_thread->strip = setup_ws2812b_strip(96);
    update_ws2812b_strip(strip_processing_thread->strip);

    // Initialize base animation values
    memset(strip_processing_thread->values_matrix, 0, sizeof(strip_processing_thread->values_matrix));

    // Set baseline thresholds
    strip_processing_thread->high_freq_divider = 35;
    strip_processing_thread->low_freq_divider = 190;
}

static inline void strip_peak_drop_decrement(led_strip_thread_processing_t *strip_process_mod)
{
    for (int x = 0; x < 12; x++)
    {
        if (strip_process_mod->values_matrix[x] > 0)
            strip_process_mod->values_matrix[x]--;
    }
}

static inline void manage_peaks_low_fq(led_strip_thread_processing_t *strip_process_mod)
{
    int num_low_clips = 0;
    int num_high_clips = 0;
    for (int k = 0; k < 2; k++)
    {
        int value = strip_process_mod->fft_data[30 + k * 8];
        value = value;
        hsv_color col;
        col.h = value / 10;
        col.v = 80;
        col.s = 255;
        value = value / strip_process_mod->low_freq_divider;

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
            if (strip_process_mod->low_freq_divider >= LOW_FREQ_LOWER_BOUNDS)
                strip_process_mod->low_freq_divider -= LOW_FREQ_DECREMENT;
        }
        if (num_high_clips >= 7)
        {
            strip_process_mod->low_freq_divider += LOW_FREQ_DECREMENT;
            if (strip_process_mod->low_freq_divider <= LOW_FREW_UPPER_BOUNDS)
                strip_process_mod->low_freq_divider = LOW_FREW_UPPER_BOUNDS;
        }

        if (strip_process_mod->values_matrix[k] < value)
            strip_process_mod->values_matrix[k] = value;

        for (int y = 0; y < strip_process_mod->values_matrix[k]; y++)
        {
            set_ws2812b_strip_hsv(strip_process_mod->strip, k * 8 + y, col);
        }
    }
}

static inline void manage_peaks_high_fq(led_strip_thread_processing_t *strip_process_mod)
{
    int num_low_clips = 0;
    int num_high_clips = 0;
    for (int k = 2; k < 12; k++)
    {
        int value = strip_process_mod->fft_data[30 + k * 8];
        value = value;
        hsv_color col;
        col.h = value / (2 * strip_process_mod->high_freq_divider / 35);
        col.v = 80;

        int saturation = 555 - (value / (strip_process_mod->high_freq_divider / 35));
        if (saturation > 255)
            saturation = 255;
        // Negative clipping
        if (saturation < 0)
            saturation = 0;
        col.s = saturation;
        value = value / strip_process_mod->high_freq_divider;

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
            if (strip_process_mod->high_freq_divider <= HIGH_FREQ_LOWER_BOUNDS)
                strip_process_mod->high_freq_divider -= HIGH_FREQ_INCREMENT;
        }
        if (num_high_clips >= 7)
        {
            strip_process_mod->high_freq_divider += HIGH_FREQ_INCREMENT;
            if (strip_process_mod->high_freq_divider >= HIGH_FREQ_UPPER_BOUNDS)
            {
                strip_process_mod->high_freq_divider = HIGH_FREQ_UPPER_BOUNDS;
            }
        }

        if (strip_process_mod->values_matrix[k] < value)
            strip_process_mod->values_matrix[k] = value;

        for (int y = 0; y < strip_process_mod->values_matrix[k]; y++)
        {
            set_ws2812b_strip_hsv(strip_process_mod->strip, k * 8 + y, col);
        }
    }
}

void led_strip_thread_one(void *params)
{
    for (;;)
    {
        fft_copy_buffer(strip_processing_thread->fft_data, sizeof(strip_processing_thread->fft_data));
        // Decrement Animation variables, unrelated to frame buffer
        strip_peak_drop_decrement(strip_processing_thread);
        // Clear contents of strip, clean slate protocol
        clear_ws2812b_strip(strip_processing_thread->strip);

        // Manage low and high peaks for the strip.
        manage_peaks_low_fq(strip_processing_thread);
        manage_peaks_high_fq(strip_processing_thread);

        // Push frame to buffer
        update_ws2812b_strip(strip_processing_thread->strip);
        usleep(10000);
    }
}
