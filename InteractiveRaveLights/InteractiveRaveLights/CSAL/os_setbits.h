#ifndef _OS_SETBITS_H
#define _OS_SETBITS_H

#include "os_error.h"
#include "pthread.h"
#include "stdbool.h"
#include "stdint.h"

typedef struct os_setbits_t{
    //OSSignal signal;
    volatile int bits;
    sem_t bit_sem;
    volatile bool sem_wait;
}os_setbits_t;

/**
 * @brief Initializes our setbits module
 * @param os_setbits_t *pointer to module
*/
int os_setbits_init(os_setbits_t *mod);

/**
 * @brief Signals a specific bit
 * @param os_setbits_t *pointer to module
*/
int os_setbits_signal(os_setbits_t *mod, uint8_t bit);

/**
 * @brief Clears a specific bit
 * @param os_setbits_t *pointer to module
*/
int os_clearbits(os_setbits_t * mod, uint8_t bit);

/**
 * @brief Wait for a specific bit to be set
 * @param os_setbits_t *pointer to module
*/
int os_waitbits(os_setbits_t *mod, uint8_t bit, uint32_t timeout_ms);

/**
 * @brief Wait indefinitely for a specific bit to be set
 * @param os_setbits_t *pointer to module
*/
int os_waitbits_indefinite(os_setbits_t *mod, uint8_t bit, uint32_t timeout_ms);

#endif