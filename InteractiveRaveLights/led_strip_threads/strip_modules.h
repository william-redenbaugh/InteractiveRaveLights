#ifndef _STRIP_MODULE_H
#define _STRIP_MODULE_H
#include "stdint.h"

void led_strip_thread_one(void *params);
void led_strip_thread_one_init(void *param);

void led_strip_thread_two(void *params);
void led_strip_thread_two_init(void *param);

void led_strip_update_module_thread(void *params);
void strip_update_module_init(void *params);
void strip_clear(void);
void strip_set_leds_hsv(int n, uint8_t h, uint8_t s, uint8_t v);
void strip_set_leds(int n, uint8_t r, uint8_t g, uint8_t b);

#endif