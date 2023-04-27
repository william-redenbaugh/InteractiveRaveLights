#ifndef _I2S_MIC_H
#define _I2S_MIC_H

/**
 * @brief Initiailization for the I2S microphone module on the Sony Spressense
*/
void i2s_mic_init(void *params);

/**
 * @brief Thread pointer declaration for I2S microphone module
*/
void i2s_mic_thread(void *params);

#endif