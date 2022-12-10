#ifndef _ADC_RUNTIME_MAIN_H
#define _ADC_RUNTIME_MAIN_H

#include "stdio.h"

void adc_runtime_thread(void *ptr);
void adc_copy_filtered_data(int16_t *input_buffer, size_t input_buffer_size);
size_t adc_filtered_data_size(void);
#endif