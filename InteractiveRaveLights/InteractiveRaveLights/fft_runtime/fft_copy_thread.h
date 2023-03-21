#ifndef _FFT_COPY_THREAD_H
#define _FFT_COPY_THREAD_H

#include "arm_math.h"

void init_fft_copy_mod(void *params);
void fft_copy_thread(void *params);
void fft_copy_buffer(q15_t *buff, size_t size);

#endif