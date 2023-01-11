/**
 * Credit for the SPI bitbang algo from gamazeps's SPIDMA ws2812b github test code
 * Link can be found here: https://github.com/gamazeps/ws2812b-chibios-SPIDMA
 * Copyright 2022: William Redenbaugh
 */
#include "ws2812b.h"
#include "hsv2rgb.h"
#include "stdio.h"
#include <nuttx/config.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <debug.h>
#include <nuttx/spi/spi_transfer.h>
#include <arch/board/board.h>
#include <arch/chip/pin.h>

#define SPI_MODE SPIDEV_MODE0
#define SPI_FREQUENCY 3000000

#define BYTES_FOR_LED_BYTE 4
#define NB_COLORS 3
#define BYTES_FOR_LED BYTES_FOR_LED_BYTE *NB_COLORS
#define DATA_SIZE BYTES_FOR_LED *NB_LEDS
#define RESET_SIZE 200

// Define the spi your LEDs are plugged to here
#define LEDS_SPI SPID2
// Define the number of LEDs you wish to control in your LED strip
#define NB_LEDS 18

#define LED_SPIRAL 0

static uint8_t get_protocol_eq(uint8_t data, int pos);

WS2812B_t *setup_ws2812b_strip(int num_leds, const char *bus)
{
    WS2812B_t *strip = malloc(sizeof(WS2812B_t));

    strip->num_leds = num_leds;
    strip->txbuff_len = RESET_SIZE + BYTES_FOR_LED * num_leds;

    strip->txbuff = malloc(sizeof(uint8_t) * (RESET_SIZE + BYTES_FOR_LED * num_leds));
    if (strip->txbuff == NULL)
    {
        printf("LED strip buffer pool not able to be allocated properly\n");
        return NULL;
    }

    strip->spi_fd = open(bus, O_WRONLY);
    if (strip->spi_fd < 0)
    {
        printf("There was an error opening up the SPI module: %d\n", strip->spi_fd);
        return NULL;
    }

    // Clear out the last bits to serve as our reset
    for (int i = 0; i < strip->txbuff_len; i++)
        strip->txbuff[i] = 0x00;

    update_ws2812b_strip(strip);
        return strip;
}

int update_ws2812b_strip(WS2812B_t *strip)
{
    int ret;
    struct spi_trans_s trans[1];
    struct spi_sequence_s seq;

    if (strip == NULL)
    {
        printf("WS2812b strip is null, not initialized properly\n");
        return -1;
    }

    if (strip->txbuff == NULL)
    {
        printf("Strip output buffer nnot valid\n");
        return -1;
    }

    seq.dev = 0;
    seq.mode = SPI_MODE;
    seq.nbits = 8;
    seq.frequency = SPI_FREQUENCY;
    seq.ntrans = 1;
    seq.trans = trans;

    trans[0].deselect = true;
    trans[0].delay = 0;
    trans[0].nwords = strip->txbuff_len;
    trans[0].rxbuffer = NULL;
    trans[0].txbuffer = strip->txbuff;

    ret = ioctl(strip->spi_fd, SPIIOC_TRANSFER, (unsigned long)(uintptr_t)&seq);
    if (ret < 0)
    {
        printf("Failure during SPI transaction... %d", ret);
    }
    return ret;
}

int set_ws2812b_strip_rgb(WS2812B_t *strip, int pos, rgb_color color)
{
    if (strip == NULL)
    {
        printf("WS2812b strip is null, not initialized properly\n");
        return -1;
    }

    if (strip->txbuff == NULL)
    {
        printf("Strip output buffer nnot valid\n");
        return -1;
    }

    if (strip->num_leds <= pos)
    {
        printf("LED selected out of bounds\n");
        return -1;
    }

    for (int j = 0; j < 4; j++)
        strip->txbuff[BYTES_FOR_LED * pos + j] = get_protocol_eq(color.g, j);
    for (int j = 0; j < 4; j++)
        strip->txbuff[BYTES_FOR_LED * pos + BYTES_FOR_LED_BYTE + j] = get_protocol_eq(color.r, j);
    for (int j = 0; j < 4; j++)
        strip->txbuff[BYTES_FOR_LED * pos + BYTES_FOR_LED_BYTE * 2 + j] = get_protocol_eq(color.b, j);
}

int clear_ws2812b_strip(WS2812B_t *strip)
{
    for (int x = 0; x < strip->num_leds; x++)
    {
        set_ws2812b_strip_rgb(strip, x, (rgb_color){0, 0, 0});
    }
}

int set_ws2812b_strip_hsv(WS2812B_t *strip, int pos, hsv_color color)
{
    set_ws2812b_strip_rgb(strip, pos, hsv2rgb(color));
}

/*
 * As the trick here is to use the SPI to send a huge pattern of 0 and 1 to
 * the ws2812b protocol, we use this helper function to translate bytes into
 * 0s and 1s for the LED (with the appropriate timing).
 */
static uint8_t get_protocol_eq(uint8_t data, int pos)
{
    uint8_t eq = 0;
    if (data & (1 << (2 * (3 - pos))))
        eq = 0b1110;
    else
        eq = 0b1000;
    if (data & (2 << (2 * (3 - pos))))
        eq += 0b11100000;
    else
        eq += 0b10000000;
    return eq;
}

/*
 * If you want to set a LED's color in the HSV color space, simply call this
 * function with a hsv_color containing the desired color and the index of the
 * led on the LED strip (starting from 0, the first one being the closest the
 * first plugged to the board)
 *
 * Only set the color of the LEDs through the functions given by this API
 * (unless you really know what you are doing)
 */
void set_led_color_hsv(hsv_color color, int pos)
{
    set_led_color_rgb(hsv2rgb(color), pos);
}
