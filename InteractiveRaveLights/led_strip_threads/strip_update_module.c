#include "strip_modules.h"
#include "ws2812b/ws2812b.h"
#include "ws2812b/hsv2rgb.h"

#include "nuttx/config.h"
#include "strip_modules.h"
#include <stdio.h>
#include "pthread.h"
#include "stdlib.h"
#include "stdint.h"

#define NUM_LEDS 192
static WS2812B_t *strip;
static pthread_mutex_t muttx;
static rgb_color strip_cols[NUM_LEDS];

void strip_set_leds(int n, uint8_t r, uint8_t g, uint8_t b){
    if(NUM_LEDS < n)
        return;

    strip_cols[n].r = r;
    strip_cols[n].g = g;
    strip_cols[n].b = b;
}

void strip_set_leds_hsv(int n, uint8_t h, uint8_t s, uint8_t v){
    if(NUM_LEDS < n)
        return;

    hsv_color hsv_col = {h, s, v};

    rgb_color rgb_col = hsv2rgb(hsv_col);

    strip_cols[n].r = rgb_col.r;
    strip_cols[n].g = rgb_col.g;
    strip_cols[n].b = rgb_col.b;
}

void strip_clear(void){
    for(int n = 0; n < NUM_LEDS; n++){
        strip_cols[n].r = 0;
        strip_cols[n].g = 0;
        strip_cols[n].b = 0;
    }
    //clear_ws2812b_strip(strip);
}

void strip_update_module_init(void *params){
    strip = strip = setup_ws2812b_strip(NUM_LEDS, "/dev/spi4");

    pthread_mutex_init(&muttx, NULL);
}

void led_strip_update_module_thread(void *params){

    for(;;){

        for(int n = 0; n < NUM_LEDS; n++){
            set_ws2812b_strip_rgb(strip, n, strip_cols[n]);
        }
        update_ws2812b_strip(strip);
        usleep(10000);
    }
}