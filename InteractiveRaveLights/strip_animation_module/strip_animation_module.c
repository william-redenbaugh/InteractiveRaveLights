#include "strip_animation_module.h"

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

    // Calculate the size of the bars
    mod->num_intervals = init.num_intervals;
    mod->interval_bar_size = mod->num_leds / mod->num_intervals;
    mod->values_matrix = malloc(mod->num_intervals * sizeof(int));

    return mod;
}
void strip_processing_runtime(strip_animation_mod_t *mod){
    for(;;){

    }
}