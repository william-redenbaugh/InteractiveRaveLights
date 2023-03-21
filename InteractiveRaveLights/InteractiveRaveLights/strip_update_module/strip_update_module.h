#ifndef _STRIP_UPDATE_MODULE
#define _STRIP_UPDATE_MODULE

#include "stdint.h"
#include "ws2812b/ws2812b.h"
#include "ws2812b/hsv2rgb.h"
#include "pthread.h"

typedef struct strip_update_thread
{
    WS2812B_t *strip;
    pthread_mutex_t muttx;
    rgb_color *strip_cols;
    int num_leds;

    void (*runtime_func)(struct strip_update_thread *mod);
} strip_update_thread_t;

void strip_set_leds_hsv(strip_update_thread_t *strip, int n, uint8_t h, uint8_t s, uint8_t v);
void strip_set_leds(strip_update_thread_t *strip, int n, uint8_t r, uint8_t g, uint8_t b);
rgb_color strip_get_leds_rgb(strip_update_thread_t *strip, int n);

extern strip_update_thread_t *strip_one_update_mod;
strip_update_thread_t *new_strip_update_thread(int num_leds, char *spi_path);

#endif