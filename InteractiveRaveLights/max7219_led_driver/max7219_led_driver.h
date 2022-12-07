#ifndef _MAX7219_LED_DRIVER_H
#define _MAX7219_LED_DRIVER_H
#include "stdio.h"

typedef struct {
    uint8_t *cols;
    uint8_t spiregister[8];
    uint8_t spidata[8];
    uint8_t out_buffer[8];
    uint8_t number_of_devices;
    uint8_t ss_pin;
    // LED matrix spi manager pointer
    int fd;
} led_matrix_t;

int init_led_matrix(led_matrix_t *matrix, int num_deivces);
void set_pixel(uint8_t x, uint8_t y, led_matrix_t *matrix);
void matrix_commit(led_matrix_t *matrix);
void clear_pixel(uint8_t x, uint8_t y, led_matrix_t *matrix);

#endif