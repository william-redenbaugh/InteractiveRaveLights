#ifndef _STRIP_ANIMATION_MODULE_H
#define _STRIP_ANIMATION_MODULE_H
#include <pthread.h>
#include "strip_update_module/strip_update_module.h"
#include "arm_math.h"
#include "nuttx/wqueue.h"

typedef enum strip_animaton_type{
    STRIP_ANIMATION_TYPE_ONE = 0
}strip_animation_type_t;


typedef struct strip_animation_mod{
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
}strip_animation_mod_t;

typedef struct{
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

}strip_animation_mod_init_t;
void strip_processing_runtime(strip_animation_mod_t *mod);
strip_animation_mod_t *new_strip_processing_mod(strip_animation_mod_init_t init);
#endif