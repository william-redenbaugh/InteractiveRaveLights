#ifndef _ADC_RUNTIME_MAIN_H
#define _ADC_RUNTIME_MAIN_H

#include "stdio.h"

/**
 * @brief before we launch the ADC thread, handle some important primitive data first
*/
void adc_init_func(void *ptr);

/**
 * @brief Runtime thread for the ADC capture module
*/
void adc_runtime_thread(void *ptr);

/**
 * @brief Copies over the data from adc thread into a destination buffer
*/
void adc_copy_filtered_data(int16_t *input_buffer, size_t input_buffer_size);

/**
 * @returns Size of ADC filtered data buffer
*/
size_t adc_filtered_data_size(void);

/**
 * @brief Blocking function for external threads to wait until new adc data is available
*/
void block_until_new_adc_data(void);
#endif