#ifndef _LED_MATRIX_GENERIC_H
#define _LED_MATRIX_GENERIC_H
#include "stdio.h"
#include "FreeRTOS.h"
#include "event_groups.h"
#include "coprocessor_ipc_shared.h"

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