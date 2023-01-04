#include "NeoPixelConnect.h"
#include "led_strip_management.h"
#include "led_matrix_generic.h"
#include "FreeRTOS.h"


// Create an instance of NeoPixelConnect and initialize it
// to use GPIO pin 4 (D12) as the control pin, for a string
// of 8 neopixels. Name the instance p
NeoPixelConnect *strip_one;

// Create an instance of NeoPixelConnect and initialize it
// to use GPIO pin 4 (D12) as the control pin, for a string
// of 8 neopixels. Name the instance p
NeoPixelConnect *strip_two;


int strip_one_set(int n, uint8_t r, uint8_t g, uint8_t b){
    //strip_one.neoPixelSetValue(n, r, g, b, false);
    return 0;
}

int strip_one_update(void){
    //strip_one.neoPixelShow();
    return 0;
}

StripGeneric_t *get_setup_strip_one(void){
    StripGeneric_t *strip = (StripGeneric_t *)pvPortMalloc(sizeof(StripGeneric_t));
    strip->strip_set = strip_one_set;
    strip->strip_update = strip_one_update;

    return strip;
}


int strip_two_set(int n, uint8_t r, uint8_t g, uint8_t b){
    //strip_two.neoPixelSetValue(n, r, g, b, false);
    return 0;
}

int strip_two_update(void){
    //strip_two.neoPixelShow();
    return 0;
}

StripGeneric_t *get_setup_strip_two(void){
    StripGeneric_t *strip = (StripGeneric_t *)pvPortMalloc(sizeof(StripGeneric_t));
    strip->strip_set = strip_two_set;
    strip->strip_update = strip_two_update;

    return strip;
}

int strip_three_set(int n, uint8_t r, uint8_t g, uint8_t b){
    return 0;
}

int strip_three_update(void){

    return 0;
}

StripGeneric_t *get_setup_strip_three(void){
    StripGeneric_t *strip = (StripGeneric_t *)pvPortMalloc(sizeof(StripGeneric_t));
    strip->strip_set = strip_three_set;
    strip->strip_update = strip_three_update;

    return strip;
}
