#ifndef _SLED1734_H_
#define _SLED1734_H_
#include <nuttx/config.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <debug.h>
#include <nuttx/i2c/i2c_master.h>
#include "rgbMatrix.h"

/**
 * @brief Gamma mapping table for animations
 */
static const uint8_t _NeoPixelGammaTable[256] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3,
    3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 5, 6, 6, 6, 6, 7,
    7, 7, 8, 8, 8, 9, 9, 9, 10, 10, 10, 11, 11, 11, 12, 12,
    13, 13, 13, 14, 14, 15, 15, 16, 16, 17, 17, 18, 18, 19, 19, 20,
    20, 21, 21, 22, 22, 23, 24, 24, 25, 25, 26, 27, 27, 28, 29, 29,
    30, 31, 31, 32, 33, 34, 34, 35, 36, 37, 38, 38, 39, 40, 41, 42,
    42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57,
    58, 59, 60, 61, 62, 63, 64, 65, 66, 68, 69, 70, 71, 72, 73, 75,
    76, 77, 78, 80, 81, 82, 84, 85, 86, 88, 89, 90, 92, 93, 94, 96,
    97, 99, 100, 102, 103, 105, 106, 108, 109, 111, 112, 114, 115, 117, 119, 120,
    122, 124, 125, 127, 129, 130, 132, 134, 136, 137, 139, 141, 143, 145, 146, 148,
    150, 152, 154, 156, 158, 160, 162, 164, 166, 168, 170, 172, 174, 176, 178, 180,
    182, 184, 186, 188, 191, 193, 195, 197, 199, 202, 204, 206, 209, 211, 213, 215,
    218, 220, 223, 225, 227, 230, 232, 235, 237, 240, 242, 245, 247, 250, 252, 255};

/**
 * @brief Handler that we use to specify which LED matrix we are using
 * @param uint8_t buffer that we put all of our RGB matrix data
 * @param int i2c_handler the handler that we use for specifying which i2c bus we are using
 */
struct I2C1735
{
    uint8_t rgb_buffer[64][3];
    int i2c_handle;
};

/**
 * @brief Initializes the hardware registers and i2c bus devices for our RGB matrix
 * @param struct I2C1735 *pointer to the handler that manages our LED matrix
 */
void RGBMatrixInit(struct I2C1735 *handle);

/**
 * @brief Fills in the entire display with a specifc color
 * @param struct I2C handler that points to the matrix device we are using
 * @param uint8_t r color value
 * @param uint8_t g color value
 * @param uint8_T b color value
 */
void disp_show_color(struct I2C1735 *handle, uint8_t R, uint8_t G, uint8_t B);

/**
 * @brief Passing a character through the display as if it's flying by
 * @param struct I2C handler that points to the matrix device we are using
 * @param uint8_t r color value
 * @param uint8_t g color value
 * @param uint8_T b color value
 */
void flowchar(struct I2C1735 *handle, char chr, uint8_t R, uint8_t G, uint8_t B);

/**
 * @brief Passes in text that flys by through the display.
 * @param struct I2C handler that points to the matrix device we are using
 * @param uint8_t r color value
 * @param uint8_t g color value
 * @param uint8_T b color value
 */
void show_text(struct I2C1735 *handle, char str[], uint8_t R, uint8_t G, uint8_t B);

/**
 * @brief Passes in text that flys by through the display. But flows in a specific direction
 * @param struct I2C handler that points to the matrix device we are using
 * @param uint8_t r color value
 * @param uint8_t g color value
 * @param uint8_T b color value
 * @param direction that we are sending the text through
 * @param delayms is the amount of time we delay bweteen sending each animation frame
 */
void flow_text(struct I2C1735 *handle, char str[], uint8_t R, uint8_t G, uint8_t B, char direction, int delayms);

/**
 * @brief Sets the background color of the matrix.
 * @param struct I2C handler that points to the matrix device we are using
 * @param uint8_t r color value
 * @param uint8_t g color value
 * @param uint8_T b color value
 */
void set_backcolor(struct I2C1735 *handle, uint8_t R, uint8_t G, uint8_t B);

/**
 * @brief Prints a hex value to the display
 * @param struct I2C handler that points to the matrix device we are using
 * @param uint8_t hex[] array of hexademical characters that we are going to send
 * @param uint8_t r color value
 * @param uint8_t g color value
 * @param uint8_T b color value
 * @param char bias
 */
void show_hex(struct I2C1735 *handle, uint8_t hex[], uint8_t R, uint8_t G, uint8_t B, char bias /*,char up_down=0*/);

/**
 * @brief Fills in the entire display with a specifc color
 * @param struct I2C handler that points to the matrix device we are using
 * @param uint32_t (c) color encoded in a 32bit integer.
 */
void disp_show_color_uint32(struct I2C1735 *handle, uint32_t c);

/**
 * @brief Fills in a specifc rgb value to a specific LED location
 * @param struct I2C handler that points to the matrix device we are using
 * @param uint8_t coor[2] Coordinates of which point we are putting on the matrix
 * @param uint8_t r color value
 * @param uint8_t g color value
 * @param uint8_T b color value
 */
void draw_point_color(struct I2C1735 *handle, uint8_t coor[2], uint8_t R, uint8_t G, uint8_t B);

/**
 * @brief Fills in a specifc rgb value to a specific LED location
 * @param struct I2C handler that points to the matrix device we are using
 * @param uint8_t coor[2] Coordinates of which point we are putting on the matrix
 * @param uint32_t c 32 bit encoded color
 */
void draw_point(struct I2C1735 *handle, uint8_t coor[2], uint32_t c);

/**
 * @brief Fills in a specifc rgb value to a line of leds on the matrix
 * @param struct I2C handler that points to the matrix device we are using
 * @param uint8_t coor[4] Coordinates of which point we are putting on the matrix(upper and lower bounds)
 * @param uint8_t r color value
 * @param uint8_t g color value
 * @param uint8_T b color value
 */
void draw_line_color(struct I2C1735 *handle, uint8_t coor[4], uint8_t R, uint8_t G, uint8_t B);

/**
 * @brief Fills in a specifc rgb value to a line of leds on the matrix
 * @param struct I2C handler that points to the matrix device we are using
 * @param uint8_t coor[4] Coordinates of which point we are putting on the matrix(upper and lower bounds)
 * @param uint32_t 32bit encoded color value
 */
void draw_line(struct I2C1735 *handle, uint8_t coor[4], uint32_t c);

/**
 * @brief Fills in a specifc rgb value to a box of leds on the matrix
 * @param struct I2C handler that points to the matrix device we are using
 * @param uint8_t coor[4] Coordinates of which point we are putting on the matrix(upper and lower bounds)
 * @param uint8_t r color value
 * @param uint8_t g color value
 * @param uint8_T b color value
 */
void draw_rectangle_color(struct I2C1735 *handle, uint8_t coor[4], uint8_t R, uint8_t G, uint8_t B);

/*!
 * @brief Fills in a specifc rgb value to a box of leds on the matrix
 * @param struct I2C handler that points to the matrix device we are using
 * @param uint8_t coor[4] Coordinates of which point we are putting on the matrix(upper and lower bounds)
 * @param uint32_t 32bit encoded color value
 */
void draw_rectangle(struct I2C1735 *handle, uint8_t coor[4], uint32_t c);

/**
 * @brief Displays the data from our matrix
 * @param struct I2C1735*handler for all our matrix related stuff
 */
void image_test(struct I2C1735 *handle);

/**
 * @brief Displays the data from our matrix
 * @param struct I2C1735*handler for all our matrix related stuff
 */
void image(struct I2C1735 *handle);

/**
 * @brief Converts an HSV color into a 32bit encoded RGB value
 * @param uint16_t hue
 * @param uint8_t saturation
 * @param uint8_t value
 * @return 32bit encoded color value
 */
uint32_t color_hsv(uint16_t hue, uint8_t sat, uint8_t val);

/**
 * @brief Gamma map
 * @param uint32_t
 */
uint32_t gamma32(uint32_t x);
#endif
