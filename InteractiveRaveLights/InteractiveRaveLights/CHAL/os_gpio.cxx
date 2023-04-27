#include "os_gpio.h"
#include "CSAL/os_error.h"
#include "stdio.h"
#include "stdlib.h"
#include "stdbool.h"
#include <arch/board/board.h>
#include <arch/chip/pin.h>


int os_gpio_config(os_gpio_conf_t *conf){
    if(conf == NULL){
        return OS_RET_NULL_PTR;
    }

    bool input = true;
    bool high_drive = false;
    int pull = 0;

    if(conf->dir == OS_GPIO_OUTPUT)
        input = false;

    if(conf->strength == OS_GPIO_HIGH_STRENGTH)
        high_drive = true;

    switch(conf->drive){
        case OS_GPIO_BUSKEEPER:
        pull = 3;
        break;

        case OS_GPIO_FLOAT: 
        pull = 0;
        break;

        case OS_GPIO_PULL_DOWN:
        pull = 2; 
        break;

        case OS_GPIO_PULL_UP:
        pull = 1;
        break;
    }

    if (board_gpio_config(conf->gpio_pin, 0, input, high_drive, pull) < 0){
        return OS_RET_INT_ERR;
    }

    return OS_RET_OK;
}

int os_gpio_get_config(os_gpio_conf_t *conf){

    uint32_t pin; 
    bool input; 
    bool output; 
    bool drive;
    int pull;

    if(conf == NULL){
        return OS_RET_NULL_PTR;
    }

    if(board_gpio_status(conf->gpio_pin, &input, &output, &drive, &pull) < 0){
        return OS_RET_INT_ERR;
    }
    
    conf->gpio_pin = pin;

    if(input){
        conf->dir = OS_GPIO_INPUT;
    }
    if(output){
        conf->dir = OS_GPIO_OUTPUT;
    }

    switch(pull){
        case 0:
        conf->drive = OS_GPIO_FLOAT;
        break;

        case 1:
        conf->drive = OS_GPIO_PULL_UP;
        break;

        case 2:
        conf->drive = OS_GPIO_PULL_DOWN;
        break;

        case 3:
        conf->drive = OS_GPIO_BUSKEEPER;
        break;
    }

    if(drive){
       conf->strength = OS_GPIO_HIGH_STRENGTH; 
    }
    else{
        conf->strength = OS_GPIO_NORMAL_STRENGTH;
    }

    return OS_RET_OK;
}

int os_gpio_isr_config(os_gpio_isr_conf_t *conf){
    if(conf == NULL){
        return OS_RET_NULL_PTR;
    }

    int mode;

    switch(conf->edge){
        case OS_ISR_HIGH_EDGE:
        mode = 2;
        break;
        case OS_ISR_LOW_EDGE:
        mode = 3;
        break;
        case OS_ISR_FALLING_EDGE:
        mode = 5;
        break;
        case OS_ISR_RISING_EDGE:
        mode = 4;
        break;
        case OS_ISR_BOTH_EDGE:
        mode = 7;
        break;
        default:
        break;
    }

    if(board_gpio_intconfig(conf->gpio_pin, mode, FALSE, (xcpt_t)conf->isr_fun) < 0){
        return OS_RET_INT_ERR;
    }

    return OS_RET_OK;
}

int os_gpio_get_isr_conf(os_gpio_isr_conf_t *conf){
    if(conf == NULL){
        return OS_RET_NULL_PTR;
    }

    int mode;
    bool filter; 
    bool enabled;

    if(board_gpio_intstatus(conf->gpio_pin, &mode, &filter, &enabled) < 0){
        return OS_RET_INT_ERR;
    }

    switch (mode)
    {
    case 2:
        conf->edge = OS_ISR_HIGH_EDGE;
        break;
    case 3:
        conf->edge = OS_ISR_LOW_EDGE;
        break;
    case 4:
        conf->edge = OS_ISR_RISING_EDGE;
        break;
    case 5: 
        conf->edge = OS_ISR_FALLING_EDGE;
        break;;

    case 7:
        conf->edge = OS_ISR_BOTH_EDGE;
    default:
        break;
    }

    conf->noise_filter = filter;
    return OS_RET_OK;
}

int os_gpio_enable_int(int pin, bool en){
    if(board_gpio_int(pin, en) < 0){
        return OS_RET_INT_ERR;
    }

    return OS_RET_OK;
}

int os_gpio_set(int gpio_pin, os_gpio_set_t set){
    board_gpio_write(gpio_pin, (int)set);
}

int os_gpio_read(int gpio_pin){
    return board_gpio_read(gpio_pin);
}