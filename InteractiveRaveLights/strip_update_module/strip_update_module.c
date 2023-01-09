#include "strip_update_module.h"

void strip_set_leds(strip_update_thread_t *strip, int n, uint8_t r, uint8_t g, uint8_t b){
    if(strip->num_leds < n)
        return;

    pthread_mutex_lock(&strip->muttx);
    strip->strip_cols[n].r = r;
    strip->strip_cols[n].g = g;
    strip->strip_cols[n].b = b;
    pthread_mutex_unlock(&strip->muttx);
}

void strip_set_leds_hsv(strip_update_thread_t *strip, int n, uint8_t h, uint8_t s, uint8_t v){
    if(strip->num_leds < n)
        return;

    hsv_color hsv_col = {h, s, v};

    rgb_color rgb_col = hsv2rgb(hsv_col);

    pthread_mutex_lock(&strip->muttx);
    strip->strip_cols[n].r = rgb_col.r;
    strip->strip_cols[n].g = rgb_col.g;
    strip->strip_cols[n].b = rgb_col.b;
    pthread_mutex_unlock(&strip->muttx);
}

static strip_update_mod_thread_runtime(strip_update_thread_t *mod){

    for(;;){

        pthread_mutex_lock(&mod->muttx);
        for(int n = 0; n < mod->num_leds; n++){
            set_ws2812b_strip_rgb(mod->strip, n, mod->strip_cols[n]);
        }
        pthread_mutex_unlock(&mod->muttx);

        update_ws2812b_strip(mod->strip);
        usleep(10000);
    }
}

strip_update_thread_t *new_strip_update_thread(int num_leds, char *spi_path){
    // Allocoate space for our strip modulese
    strip_update_thread_t *head = malloc(sizeof(strip_update_thread_t));

    // Setup LED strip
    head->strip = setup_ws2812b_strip(num_leds, spi_path);
    head->num_leds = num_leds;
    head->strip_cols = malloc(sizeof(rgb_color) * num_leds);
    head->runtime_func = strip_update_mod_thread_runtime;

    // Initialize the mutex.
    pthread_mutex_init(&head->muttx, NULL);

    return head;
}