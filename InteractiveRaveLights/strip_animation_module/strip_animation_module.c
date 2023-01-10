#include "strip_animation_module.h"
#include "led_strip_threads/strip_modules.h"
#include "fft_runtime/fft_copy_thread.h"

strip_animation_mod_t *new_strip_processing_mod(strip_animation_mod_init_t init){
    strip_animation_mod_t *mod = malloc(sizeof(strip_animation_mod_t));

    mod->current_animation = init.init_type;
    mod->high_freq_divider = 55;
    mod->low_freq_divider = 600;
    mod->strip = init.strip;
    mod->strip_pos_lower_bounds = init.strip_lower_bounds;
    mod->strip_pos_upper_bounds = init.strip_upper_bounds;
    mod->num_leds = init.strip_upper_bounds - init.strip_lower_bounds;
    mod->fft_data_size = init.fft_data_size;
    mod->fft_data = malloc(sizeof(q15_t) * init.fft_data_size);
    mod->fft_copy_buffer = init.fft_copy_buffer;
    // Calculate the size of the bars
    mod->num_intervals = init.num_intervals;
    mod->interval_bar_size = mod->num_leds / mod->num_intervals;
    mod->values_matrix = malloc(mod->num_intervals * sizeof(int));
    mod->hue_offset = init.hue_offset;
    return mod;
}

static inline void strip_peak_drop_decrement(strip_animation_mod_t *mod){
    for(int x = 0; x < mod->num_intervals; x++)
        if(mod->values_matrix[x] > 0)
            mod->values_matrix--;
}

static inline void clear_strip(strip_animation_mod_t *mod){
    for(int n = 0; n < mod->num_leds; n++){
        strip_set_leds(mod->strip, n, 0, 0, 0);
    }
}

static inline void manage_peaks(strip_animation_mod_t *mod){
    for(int n = 0; n < mod->num_intervals; n++){
        int value  = mod->fft_data[30 + n * 8];
        hsv_color col;
        col.h = value / (2 * mod->high_freq_divider / 35);
        col.v = 30;

        int saturation = 555 - (value / (mod->high_freq_divider / 35));
        if (saturation > 255)
            saturation = 255;
        // Negative clipping
        if (saturation < 0)
            saturation = 0;
        col.s = saturation;
        value = value / mod->high_freq_divider;

        if(mod->values_matrix[n] < value)
            mod->values_matrix[n] = value;

        for(int y = 0; y < mod->values_matrix[n]; y++){
            strip_set_leds(mod->strip, n * 8 + y, col.h, col.s, col.v);
        }
    }
}

static inline void animation_type_one(strip_animation_mod_t *mod){
    // Copy fft data contents into buff
    mod->fft_copy_buffer(mod->fft_data, mod->fft_data_size);
    strip_peak_drop_decrement(mod);
    clear_strip(mod);
    usleep(10000);
}

void strip_processing_runtime(strip_animation_mod_t *mod){
    for(;;){
        switch(mod->current_animation){
            case STRIP_ANIMATION_TYPE_ONE:
            animation_type_one(mod);
            break;
            default:
            break;
        }
    }
}