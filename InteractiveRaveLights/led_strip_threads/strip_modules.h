#ifndef _STRIP_MODULE_H
#define _STRIP_MODULE_H
#include "stdint.h"
#include "ws2812b/ws2812b.h"
#include "ws2812b/hsv2rgb.h"
#include "pthread.h"
#include "strip_update_module/strip_update_module.h"

#define NUM_LEDS 192

void led_strip_thread_one(void *params);
void led_strip_thread_one_init(void *param);

void led_strip_thread_two(void *params);
void led_strip_thread_two_init(void *param);

// For managing our strip
extern strip_update_thread_t *strip_mod;
void led_strip_update_module_thread(void *params);
void strip_update_module_init(void *params);
#endif