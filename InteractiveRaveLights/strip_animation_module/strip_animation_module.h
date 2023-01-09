#ifndef _STRIP_ANIMATION_MODULE_H
#define _STRIP_ANIMATION_MODULE_H
#include <pthread.h>
#include "strip_update_module/strip_update_module.h"
#include "arm_math.h"

typedef enum strip_animaton_type{
    STRIP_ANIMATION_TYPE_ONE = 0
}strip_animation_type_t;


typedef struct strip_animation_mod{
    strip_animation_type_t current_animation;
    q15_t *fft_data;
    size_t fft_data_size;
    int *values_matrix;
    int low_freq_divider;
    int high_freq_divider;
    strip_update_thread_t *strip;
    int strip_pos_upper_bounds;
    int strip_pos_lower_bounds;
    int num_leds;
    int num_intervals;
    int interval_bar_size;
}strip_animation_mod_t;

typedef struct{
    int strip_upper_bounds;
    int strip_lower_bounds;
    strip_update_thread_t *strip;
    strip_animation_type_t init_type;
    size_t fft_data_size;

    int num_intervals;
}strip_animation_mod_init_t;

#endif