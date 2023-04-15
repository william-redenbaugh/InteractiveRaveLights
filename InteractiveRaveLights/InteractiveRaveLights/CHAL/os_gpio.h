#ifndef _OS_GPIO_H
#define _OS_GPIO_H

typedef struct os_gpio_t{
    int n;
}os_gpio_t;

typedef enum os_gpio_drive_t{
    OS_GPIO_FLOAT = 0, 
    OS_GPIO_PULL_UP,
    OS_GPIO_PULL_DOWN,  
    OS_GPIO_BUSKEEPER
}os_gpio_drive_t;

typedef enum os_gpio_direction_t{
    OS_GPIO_INPUT, 
    OS_GPIO_OUTPUT
}os_gpio_direction_t;

typedef enum os_gpio_set_t{
    OS_GPIO_HIGH, 
    OS_GPIO_LOW
}os_gpio_set_t;

typedef enum os_gpio_isr_edge_t{
    OS_ISR_HIGH_EDGE, 
    OS_ISR_LOW_EDGE,  
    OS_ISR_RISING_EDGE,
    OS_ISR_FALLING_EDGE,
    OS_ISR_BOTH_EDGE
}os_gpio_isr_edge_t;

typedef struct os_gpio_conf_t{
    os_gpio_drive_t drive;
    os_gpio_direction_t dir;
    int gpio_pin;
}os_gpio_conf_t;

typedef void(*isr_func_t)(void*);

typedef struct os_gpio_isr_conf_t{
    int gpio_pin;
    os_gpio_isr_edge_t edge;
    isr_func_t isr_fun;
}os_gpio_isr_conf_t;

/**
 * @brief 
*/
int os_gpio_config(os_gpio_conf_t *conf);

/**
 * @brief 
*/
int os_gpio_isr_config(os_gpio_isr_conf_t *conf);

/**
 * @brief
*/
int os_gpio_set(int gpio_pin, os_gpio_set_t);

/**
 * @brief
*/
int os_gpio_read(int gpio_pin);
#endif