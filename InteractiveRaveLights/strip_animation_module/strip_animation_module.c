#include "strip_animation_module.h"
#include "led_strip_threads/strip_modules.h"
#include "fft_runtime/fft_copy_thread.h"
#include "strip_update_module/strip_update_module.h"
#include "nuttx/wqueue.h"

#define PEAK_MANAGEMENT_INTERVAL_MS 5000

static void peak_management_wq(int argc, char *argv[]){
    strip_animation_mod_t *mod = (strip_animation_mod_t*)argv;
    printf("Got this far\n");
    //int ret = work_queue(LPWORK, &mod->peak_management_wq_handle, (worker_t)peak_management_wq, arg, MSEC2TICK(PEAK_MANAGEMENT_INTERVAL_MS));
}

strip_animation_mod_t *new_strip_processing_mod(strip_animation_mod_init_t init){
    strip_animation_mod_t *mod = malloc(sizeof(strip_animation_mod_t));
    // Setup mutex
    pthread_mutex_init(&mod->animation_mttx, NULL);
    // animation speed
    mod->delay_frametime_us = init.delay_frametime_us;

    // Which animation we are using!
    mod->current_animation = init.init_type;

    // FFT copy buffer for dealing with animations!
    mod->fft_copy_buffer = init.fft_copy_buffer;
    mod->fft_data_size = init.fft_data_size;
    mod->fft_data = malloc(sizeof(q15_t) * mod->fft_data_size);

    // How many bars are we using?
    mod->num_intervals = init.num_intervals;

    // Peaks matrix and peaks divider( apply divider across entire spectrum)
    mod->values_matrix = malloc(mod->num_intervals * sizeof(int));
    mod->peaks_matrix = malloc(mod->num_intervals * sizeof(int));
    memset(mod->values_matrix, 0, sizeof(int) * mod->num_intervals);
    memset(mod->peaks_matrix, 0, sizeof(int) * mod->num_intervals);

    mod->high_freq_divider = 100;
    mod->low_freq_divider = 600;

    mod->strip = strip_mod;
    mod->strip_pos_lower_bounds = init.strip_lower_bounds;
    mod->strip_pos_upper_bounds = init.strip_upper_bounds;

    // calculate number LEDS based off upper and lower bounds
    mod->num_leds = init.strip_upper_bounds - init.strip_lower_bounds;
    mod->interval_bar_size = mod->num_leds / mod->num_intervals;

    // Hue shift as song intensity increases
    mod->hue_high = init.hue_high;
    mod->hue_low = init.hue_low;
    mod->hue_divider = (mod->hue_high - mod->hue_low) / mod->interval_bar_size;

    // Helps manage animation for brightness as intensity increases
    mod->brightness_high = init.brightness_high;
    mod->brightness_low = init.brightness_low;
    mod->brightness_divider = (mod->brightness_high - mod->brightness_low) / mod->interval_bar_size;

    // Helps manage animation for saturation as volume increases
    mod->saturation_high = init.saturation_high;
    mod->saturation_low = init.saturation_low;
    mod->saturation_divider = (mod->saturation_high - mod->saturation_low) / mod->interval_bar_size;
    return mod;
}


static inline void strip_peak_drop_decrement(strip_animation_mod_t *mod){
    for(int x = 0; x < 12; x++)
        if(mod->values_matrix[x] > 0)
            mod->values_matrix[x]--;
}

static inline void clear_strip(strip_animation_mod_t *mod){
    for(int n = mod->strip_pos_lower_bounds; n < mod->strip_pos_upper_bounds; n++){
        strip_set_leds(mod->strip, n, 0, 0, 0);
    }
}

static hsv_color calculate_bar_hsv(strip_animation_mod_t *mod, int value, int n){
    hsv_color col;
    value = value / mod->high_freq_divider;

    if(value > mod->interval_bar_size)
        value = mod->interval_bar_size;

    if(mod->values_matrix[n] <= value)
        mod->values_matrix[n] = value;

    col.h = mod->hue_low + mod->values_matrix[n] * mod->hue_divider;
    col.s = mod->saturation_high - (mod->values_matrix[n] * mod->saturation_divider);
    col.v = mod->brightness_low + mod->values_matrix[n] * mod->brightness_divider;

    return col;
}

static void animation_type_one_manage_peaks(strip_animation_mod_t *mod){
    for(int n = 0; n <  mod->num_intervals; n++){
        hsv_color col = calculate_bar_hsv(mod, mod->fft_data[31 + n * mod->interval_bar_size], n);

        for(int y = 0; y < mod->values_matrix[n]; y++){
            strip_set_leds_hsv(mod->strip, mod->strip_pos_lower_bounds + n * mod->interval_bar_size + y, col.h, col.s, col.v);
        }
        for(int y = mod->values_matrix[n] - 1; y < mod->interval_bar_size -1; y++){
            strip_set_leds(mod->strip, mod->strip_pos_lower_bounds + n * mod->interval_bar_size + y, 0, 0, 0);
        }
    }
}

static void strip_manage_bars(strip_animation_mod_t *mod){
    for(int n = 0; n <  mod->num_intervals; n++){
        hsv_color col = calculate_bar_hsv(mod, mod->fft_data[31 + n * mod->interval_bar_size], n);

        for(int y = 0; y < mod->interval_bar_size - 1; y++){
            strip_set_leds_hsv(mod->strip, mod->strip_pos_lower_bounds + n * mod->interval_bar_size + y, col.h, col.s, col.v);
        }
    }
}


static inline void animation_type_one(strip_animation_mod_t *mod){
    // Copy fft data contents into buff
    strip_peak_drop_decrement(mod);
    animation_type_one_manage_peaks(mod);
}

static inline void animation_type_two(strip_animation_mod_t *mod){
    strip_peak_drop_decrement(mod);
    strip_manage_bars(mod);

}

void strip_processing_runtime(strip_animation_mod_t *mod){
    for(;;){
        mod->fft_copy_buffer(mod->fft_data, mod->fft_data_size);
        pthread_mutex_lock(&mod->animation_mttx);
        switch(mod->current_animation){
            case STRIP_ANIMATION_END:
                clear_strip(mod);

            case STRIP_ANIMATION_TYPE_ONE:
            animation_type_one(mod);
            break;

            case STRIP_ANIMATION_TYPE_TWO:
            animation_type_two(mod);
            default:
            break;
        }
        pthread_mutex_unlock(&mod->animation_mttx);
        usleep(mod->delay_frametime_us);
    }
}

void stop_animation(strip_animation_mod_t *mod){
    pthread_mutex_lock(&mod->animation_mttx);
    mod->current_animation = STRIP_ANIMATION_END;
    pthread_mutex_unlock(&mod->animation_mttx);
}