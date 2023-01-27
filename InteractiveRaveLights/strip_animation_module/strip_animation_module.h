#ifndef _STRIP_ANIMATION_MODULE_H
#define _STRIP_ANIMATION_MODULE_H
#include <pthread.h>
#include "strip_update_module/strip_update_module.h"
#include "arm_math.h"
#include "nuttx/wqueue.h"

/**
 * @brief Different animation types to set for the strip animation module
 */
typedef enum strip_animaton_type
{
    STRIP_ANIMATION_OFF = 0,
    STRIP_ANIMATION_TYPE_ONE,
    STRIP_ANIMATION_TYPE_TWO,
    STRIP_ANIMATION_TYPE_THREE,
    STRIP_ANIMATION_TYPE_FADE_OUT,
    STRIP_ANIMATION_TYPE_FADE_RAINBOW,
    STRIP_ANIMATION_TYPE_RAINBOW_ONE
} strip_animation_type_t;

typedef struct fade_out_vals
{
    bool leds_strip_active;
} fade_out_vals_t;

typedef struct static_animation_vals{
    uint8_t current_h_pos;
    uint8_t current_s_pos;
    uint8_t current_v_pos;
}static_animation_vals_t;

/**
 * @brief Module that contains all the variables needed for strip animations
 * @note Must be used by calling  "new_strip_processing_mod", do not make your own structure
 */
typedef struct strip_animation_mod
{
    pthread_mutex_t animation_mttx;
    bool animation_changed;
    int delay_frametime_us;
    strip_animation_type_t current_animation;
    void (*fft_copy_buffer)(q15_t *buff, size_t size);
    size_t fft_data_size;
    q15_t *fft_data;
    int num_intervals;
    struct work_s peak_management_wq_handle;
    int *values_matrix;
    int *peaks_matrix;
    int low_freq_divider;
    int high_freq_divider;
    strip_update_thread_t *strip;
    int strip_pos_upper_bounds;
    int strip_pos_lower_bounds;
    int num_leds;
    int interval_bar_size;
    int hue_high;
    int hue_low;
    int saturation_low;
    int saturation_high;
    int hue_divider;
    int saturation_divider;
    int brightness_low;
    int brightness_high;
    int brightness_divider;

    fade_out_vals_t fade_out_vals;
    static_animation_vals_t animation_vals;
} strip_animation_mod_t;

/**
 * @brief Initialization structure for strip animations
 * @param Configure to initialize a strip animation module, then pass configuration into "new_strip_processing_mod"
 * @param int delay_frametime_us: How much time to delay between animation frames
 * @param int strip_upper_bounds: highest segment of strip we are using
 * @param int strip_lower_bounds: Lowest segment of strip we are using
 * @param int hue/saturation/brightness_high: during animation, this is the highest peak we'll reach for during higher intensity
 * @param int hue/saturation/brightness_low: during animation this is the lowest_peak we'll reach during lower intensity
 * @param strip_update_thread_t* strip pointer to strip update management system we are using
 * @param strip_animation_t init_type starting animation
 * @param size_t fft_data_size: How big is the buffer we will copy over
 * @param int num_intervals: how many segments we want for our animation
 * @param fft_copy_buffer: Pointer to function where we'll copy our fft data from, must be async friendly!
 */
typedef struct
{
    int delay_frametime_us;
    int strip_upper_bounds;
    int strip_lower_bounds;
    int hue_high;
    int hue_low;
    int saturation_low;
    int saturation_high;
    int brightness_low;
    int brightness_high;
    strip_update_thread_t *strip;
    strip_animation_type_t init_type;
    size_t fft_data_size;
    int num_intervals;
    void (*fft_copy_buffer)(q15_t *buff, size_t size);

} strip_animation_mod_init_t;

/**
 * @brief Wherever you put the processing runtime thread, let this be the only function
 * you call in there, this function shouldn't ever return
 * @param strip_animation_mod_t* pointer to the strip_animation handler
 */
void strip_processing_runtime(strip_animation_mod_t *mod);

/**
 * @brief Instantiates the strip processing runtime variables
 * @param strip_animation_mod_init_t structure with all animation variables
 * @return strip_animation_mod_t* pointer to allocated memory used as structure for strip animation module
 * @note please populate init structure with correct variables based off design criteria
 */
strip_animation_mod_t *new_strip_processing_mod(strip_animation_mod_init_t init);

/**
 * @brief Stops the animation, makes strip segment go blank
 * @param strip_animation_mod_t* pointer to the correct strip animation
 */
void stop_animation(strip_animation_mod_t *mod);
#endif