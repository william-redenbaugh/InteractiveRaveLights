#include "led_matrix_runtime.h"
#include <nuttx/config.h>
#include <stdio.h>
#include "thread_init/threads_init.h"
#include "pthread.h"
#include "pubsub-c/pubsub.h"
#include "max7219_led_driver/max7219_led_driver.h"

void led_matrix_runtime(void *ptr){
    led_matrix_t matrix;

    printf("LED Matrix!\n");

    int ret = init_led_matrix(&matrix, 4);
    if (ret < 0){
        printf("Error setting up MAX7219\n");
    }

    set_pixel(0, 0, &matrix);
    matrix_commit(&matrix);

    printf("LED Matrix!\n");
    for(;;){
        usleep(100000);
    }
}