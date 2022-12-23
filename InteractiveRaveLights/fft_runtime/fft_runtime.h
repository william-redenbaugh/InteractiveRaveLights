#ifndef _FFT_RUNTIME_MAIN_H
#define _FFT_RUNTIME_MAIN_H
#include "arm_math.h"
#include "arm_const_structs.h"

void fft_primary_task(void *ptr);
void fft_module_init(void);
void block_until_new_fft_data(void);
void fft_copy_data(q15_t *data_ptr, size_t size);

#endif