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
#include "strip_animation_module/strip_animation_module.h"

static strip_animation_mod_t *ani_mod;

strip_animation_mod_t *setup_strip_processing_mode(int lower_bounds, int upper_bounds){
    strip_animation_mod_init_t init_config;

    init_config.fft_data_size = 128 * sizeof(q15_t);
    init_config.num_intervals = 12;
    init_config.strip = strip_mod;
    init_config.init_type = STRIP_ANIMATION_TYPE_ONE;
    init_config.fft_copy_buffer = fft_copy_buffer;
    init_config.strip_upper_bounds = upper_bounds;
    init_config.strip_lower_bounds = lower_bounds;

    init_config.hue_high = 120;
    init_config.hue_low = 80;

    init_config.brightness_high = 30;
    init_config.brightness_low = 30;

    init_config.saturation_high = 255;
    init_config.saturation_low = 200;

    // Instanciate!
    strip_animation_mod_t *strip_one_animation = new_strip_processing_mod(init_config);

    return strip_one_animation;
}

void led_strip_thread_one_init(void *params)
{
    ani_mod = setup_strip_processing_mode(0, 96);
}

void led_strip_thread_one(void *params)
{
    strip_processing_runtime(ani_mod);
}