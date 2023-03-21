#include "strip_modules.h"
#include "ws2812b/ws2812b.h"
#include "ws2812b/hsv2rgb.h"

#include "nuttx/config.h"
#include "strip_modules.h"
#include <stdio.h>
#include "pthread.h"
#include "stdlib.h"
#include "stdint.h"
#include "strip_update_module/strip_update_module.h"

strip_update_thread_t *strip_mod;

void strip_update_module_init(void *params){
    strip_mod = new_strip_update_thread(NUM_LEDS, "dev/spi4");
}

void led_strip_update_module_thread(void *params){
    strip_mod->runtime_func(strip_mod);
}