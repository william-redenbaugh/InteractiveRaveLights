#ifndef _WS2812B_H
#define _WS2812B_H

#include "stdio.h"
#include "hsv2rgb.h"

typedef struct WS2812B
{
    uint8_t *txbuff;
    int txbuff_len;
    int spi_fd;
    int num_leds;
} WS2812B_t;

/**!
 * @brief Initializes the strip based off of the amount of LEDs that we want
 * @param int num_leds to initialize
 * @return WS2812b struct pointer, or NULL otherwise
 */
WS2812B_t *setup_ws2812b_strip(int num_leds, const char *bus);

/**!
 * @brief Pushes the contents inside of the ws2812b buffer through the SPI interface
 * @param WS2812b_t *strip pointer instance
 * @return errno return value for status
 */
int update_ws2812b_strip(WS2812B_t *strip);

/**!
 * @brief Allows us to set specific LEDS to specific RGB values for the WS2812b  LED matrix
 * @param WS2812b_t *strip pointer of instance
 * @param int pos position of LED on strip, indexed at 0
 * @param rgb_color color we are changing LED to
 * @return errno return value for status
 */
int set_ws2812b_strip_rgb(WS2812B_t *strip, int pos, rgb_color color);

/**!
 * @brief Allows us to set specific LEDS to specific RGB values for the WS2812b  LED matrix
 * @param WS2812b_t *strip pointer of instance
 * @param int pos position of LED on strip, indexed at 0
 * @param hsv_color color we are changing LED to
 * @return errno return value for status
 */
int set_ws2812b_strip_hsv(WS2812B_t *strip, int pos, hsv_color color);

/**!
 * @brief Clears the strip's rgb values
 * @param WS2812b_t *strip pointer of instance
 */
int clear_ws2812b_strip(WS2812B_t *strip);
#endif