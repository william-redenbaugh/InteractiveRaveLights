#include "strip_modules.h"
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
#include "nuttx/wqueue.h"
#include "pthread.h"

/**
 * Localized declarations
 */
static const int LOW_FREQ_LOWER_BOUNDS = 10;
static const int LOW_FREW_UPPER_BOUNDS = 1000;
static const int HIGH_FREQ_LOWER_BOUNDS = 5;
static const int HIGH_FREQ_UPPER_BOUNDS = 700;
static const int HIGH_FREQ_INCREMENT = 1;
static const int LOW_FREQ_DECREMENT = 5;

#define PEAK_DETECTION_WQ_INTERVAL 6000
static struct work_s peak_detection_calc;

/**
 * @brief Localized Module Variables
 */

typedef struct led_strip_thread_processing
{
    q15_t fft_data[ADC_FFT_BUFFER_SIZE];
    int values_matrix[12];
    //WS2812B_t *strip;
    int low_freq_divider;
    int high_freq_divider;
} led_strip_thread_processing_t;

static led_strip_thread_processing_t *strip_processing_thread;
static int high_peaks_low_freq = 0;
static int low_peaks_low_freq = 0;
static pthread_mutex_t low_freq_peak_mtx;

static int high_peaks_high_freq = 0;
static int low_peaks_high_freq = 0;
static pthread_mutex_t high_freq_peak_mtx;

static void peak_detection_offset_wq(int argc, char*argv[]){

    pthread_mutex_lock(&low_freq_peak_mtx);

    printf("Peaks detected: Low %d, High %d\n", low_peaks_high_freq, high_peaks_high_freq);

    pthread_mutex_unlock(&low_freq_peak_mtx);

    pthread_mutex_lock(&high_freq_peak_mtx);

    pthread_mutex_unlock(&high_freq_peak_mtx);

    int ret = work_queue(LPWORK, &peak_detection_calc, (worker_t)peak_detection_offset_wq, NULL, MSEC2TICK(PEAK_DETECTION_WQ_INTERVAL));
}

void led_strip_thread_one_init(void *params)
{
    // Allocate space for our thread processing module
    strip_processing_thread = malloc(sizeof(led_strip_thread_processing_t));

    // Initialize strip
    //strip_processing_thread->strip = setup_ws2812b_strip(96, "/dev/spi4");
    //update_ws2812b_strip(strip_processing_thread->strip);

    // Initialize base animation values
    memset(strip_processing_thread->values_matrix, 0, sizeof(strip_processing_thread->values_matrix));

    // Set baseline thresholds
    strip_processing_thread->high_freq_divider = 55;
    strip_processing_thread->low_freq_divider = 600;

    pthread_mutex_init(&low_freq_peak_mtx, NULL);
    pthread_mutex_init(&high_freq_peak_mtx, NULL);

    int ret = work_queue(LPWORK, &peak_detection_calc, (worker_t)peak_detection_offset_wq, NULL, MSEC2TICK(PEAK_DETECTION_WQ_INTERVAL));
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

        pthread_mutex_lock(&low_freq_peak_mtx);
        if (value > 7)
        {
            high_peaks_low_freq++;
            value = 7;
        }
        if (value == 0)
        {
            low_peaks_low_freq++;
        }
        pthread_mutex_unlock(&low_freq_peak_mtx);

        if (strip_process_mod->values_matrix[k] < value)
            strip_process_mod->values_matrix[k] = value;

        for (int y = 0; y < strip_process_mod->values_matrix[k]; y++)
        {
            strip_set_leds_hsv(strip_mod, k * 8 + y, col.h, col.s, col.v);
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
        col.v = 30;

        int saturation = 555 - (value / (strip_process_mod->high_freq_divider / 35));
        if (saturation > 255)
            saturation = 255;
        // Negative clipping
        if (saturation < 0)
            saturation = 0;
        col.s = saturation;
        value = value / strip_process_mod->high_freq_divider;

        pthread_mutex_lock(&high_freq_peak_mtx);
        if (value > 7)
        {
            high_peaks_high_freq++;
            value = 7;
        }
        if (value == 0)
        {
            low_peaks_high_freq++;
        }
        pthread_mutex_unlock(&high_freq_peak_mtx);


        if (strip_process_mod->values_matrix[k] < value)
            strip_process_mod->values_matrix[k] = value;

        for (int y = 0; y < strip_process_mod->values_matrix[k]; y++)
        {
            strip_set_leds_hsv(strip_mod, k * 8 + y, col.h, col.s, col.v);
            //set_ws2812b_strip_hsv(strip_process_mod->strip, k * 8 + y, col);
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
        //clear_ws2812b_strip(strip_processing_thread->strip);
        for(int n = 0; n < 96; n++)
            strip_set_leds(strip_mod, n, 0, 0, 0);
        // Manage low and high peaks for the strip.
        manage_peaks_low_fq(strip_processing_thread);
        manage_peaks_high_fq(strip_processing_thread);

        // Push frame to buffer
        //update_ws2812b_strip(strip_processing_thread->strip);
        usleep(10000);
    }
}
