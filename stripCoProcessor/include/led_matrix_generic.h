#ifndef _LED_MATRIX_GENERIC_H
#define _LED_MATRIX_GENERIC_H
#include "stdio.h"
#include "FreeRTOS.h"
#include "event_groups.h"

#define NUM_PIXELS_PER_STRIP 96
#define NUM_COLS_PER_PIXEL 3
#define NUM_STRIPS 3
#define INPUT_BUFFER_SIZE NUM_PIXELS_PER_STRIP *NUM_COLS_PER_PIXEL *NUM_STRIPS

typedef struct StripGeneric
{
    int (*strip_set)(int n, uint8_t r, uint8_t g, uint8_t b);
    int (*strip_update)(void);
} StripGeneric_t;

typedef struct UpdateStripStruct
{
    StripGeneric_t *strip;
    int num_leds;
    uint8_t *buffer;
    SemaphoreHandle_t mutex;
    EventGroupHandle_t new_data_group;
} UpdateStripStruct_t;

typedef struct RgbColor
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
} rgb_color_t;

#define BIT_0 (1 << 0)
#endif