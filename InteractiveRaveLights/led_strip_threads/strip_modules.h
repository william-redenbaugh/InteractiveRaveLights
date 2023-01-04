#ifndef _STRIP_MODULE_H
#define _STRIP_MODULE_H

void led_strip_thread_one(void *params);
void led_strip_thread_one_init(void *param);

void led_strip_thread_two(void *params);
void led_strip_thread_two_init(void *param);


#endif