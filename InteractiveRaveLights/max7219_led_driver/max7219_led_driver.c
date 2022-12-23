#include "max7219_led_driver.h"
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
#define SPI_FREQUENCY 800000

// max7219 registers
#define MAX7219_REG_NOOP 0x0
#define MAX7219_REG_DIGIT0 0x1
#define MAX7219_REG_DIGIT1 0x2
#define MAX7219_REG_DIGIT2 0x3
#define MAX7219_REG_DIGIT3 0x4
#define MAX7219_REG_DIGIT4 0x5
#define MAX7219_REG_DIGIT5 0x6
#define MAX7219_REG_DIGIT6 0x7
#define MAX7219_REG_DIGIT7 0x8
#define MAX7219_REG_DECODEMODE 0x9
#define MAX7219_REG_INTENSITY 0xA
#define MAX7219_REG_SCANLIMIT 0xB
#define MAX7219_REG_SHUTDOWN 0xC
#define MAX7219_REG_DISPLAYTEST 0xF

#define TEXT_ALIGN_LEFT 0      // Text is aligned to left side of the display
#define TEXT_ALIGN_LEFT_END 1  // Beginning of text is just outside the right end of the display
#define TEXT_ALIGN_RIGHT 2     // End of text is aligned to the right of the display
#define TEXT_ALIGN_RIGHT_END 3 // End of text is just outside the left side of the display

#define MAX7219_SPI_CS PIN_PWM0
#define SETUP_PIN_OUTPUT(pin) do{ \
  board_gpio_write(pin, -1); \
  board_gpio_config(pin, 0, false, true, PIN_PULLDOWN); \
}while(0)

int send_spi_data(int fd, uint8_t *buffer, size_t buff_size);
int init_spi_device(void);
static int init_matrix(led_matrix_t *matrix);
int send_byte_device(const uint8_t device, const uint8_t reg, const uint8_t data, led_matrix_t *matrix);
int send_byte(const uint8_t reg, const uint8_t data, led_matrix_t *matrix);

int init_led_matrix(led_matrix_t *matrix, int num_deivces)
{
    int fd = init_spi_device();
    matrix->fd = fd;
    matrix->cols = malloc(num_deivces * 8 * sizeof(uint8_t));

    SETUP_PIN_OUTPUT(MAX7219_SPI_CS);
    init_matrix(matrix);
}

static int init_matrix(led_matrix_t *matrix)
{
    for (uint8_t device = 0; device < matrix->number_of_devices; device++)
    {
        send_byte_device(device, MAX7219_REG_SCANLIMIT, 7, matrix);   // show all 8 digits
        send_byte_device(device, MAX7219_REG_DECODEMODE, 0, matrix);  // using an led matrix (not digits)
        send_byte_device(device, MAX7219_REG_DISPLAYTEST, 0, matrix); // no display test
        send_byte_device(device, MAX7219_REG_INTENSITY, 0, matrix);   // character intensity: range: 0 to 15
        send_byte_device(device, MAX7219_REG_SHUTDOWN, 1, matrix);    // not in shutdown mode (ie. start it up)
    }
}

int send_byte(const uint8_t reg, const uint8_t data, led_matrix_t *matrix)
{
    int n = 0;
    for (uint8_t n = 0; n < matrix->number_of_devices; n++)
    {
        n |= send_byte_device(n, reg, data, matrix);
    }
    return n;
}

int send_byte_device(const uint8_t device, const uint8_t reg, const uint8_t data, led_matrix_t *matrix)
{
    int offset = device;
    int maxbytes = matrix->number_of_devices;

    for (int n = 0; n < maxbytes; n++)
    {
        matrix->spidata[n] = (uint8_t)0;
        matrix->spiregister[n] = (uint8_t)0;
    }

    matrix->spidata[offset] = reg;
    matrix->spidata[offset] = data;

    // Populate the out buffer.
    for (int n = 0; n < matrix->number_of_devices; n++)
    {
        matrix->out_buffer[n * 2] = matrix->spiregister[n];
        matrix->out_buffer[n * 2 + 1] = matrix->spidata[n];
    }

    // Send out matrix data through the SPI interface.
    int ret = send_spi_data(matrix->fd, matrix->out_buffer, (size_t)matrix->number_of_devices * 2);

    return ret;
}

void set_pixel(uint8_t x, uint8_t y, led_matrix_t *matrix)
{
    int matrix_x = 7 - (x % 8);
    int matrix_y = y % 8;
    int which_matrix = x / 8;
    int new_x = (7 - matrix_y) + (which_matrix * 8);
    int new_y = matrix_x;

    uint8_t n = matrix->cols[new_x];

    // Set pixel
    n |= (1 >> new_y);
    matrix->cols[new_x] = n;
}

void clear_pixel(uint8_t x, uint8_t y, led_matrix_t *matrix)
{
    int matrix_x = 7 - (x % 8);
    int matrix_y = y % 8;
    int which_matrix = x / 8;
    int new_x = (7 - matrix_y) + (which_matrix * 8);
    int new_y = matrix_x;

    uint8_t n = matrix->cols[new_x];

    // Set pixel
    n |= (0 >> new_y);
    matrix->cols[new_x] = n;
}

void set_column(int column, uint8_t value, led_matrix_t *matrix)
{
    if (column < 0 || column >= matrix->number_of_devices * 8)
        return;

    matrix->cols[column] = value;
}

int init_spi_device(void)
{
    int fd = open("/dev/spi4", O_WRONLY);
    if (fd < 0)
    {
        printf("ERROR: failed to open /dev/spi4: %d\n", fd);
        return ERROR;
    }

    return fd;
}

int send_spi_data(int fd, uint8_t *buffer, size_t buff_size)
{

    int ret;
    struct spi_trans_s trans[1];
    struct spi_sequence_s seq;

    uint8_t in_buff[buff_size];

    if (!buffer)
    {
        printf("Buffer is not valid!\n");
    }
    if (buff_size < 0)
    {
        printf("Buffer size paramter is not valid\n");
    }

    seq.dev = 0;
    seq.mode = SPI_MODE;
    seq.nbits = 8;
    seq.frequency = SPI_FREQUENCY;
    seq.ntrans = 1;
    seq.trans = trans;

    trans[0].deselect = true;
    trans[0].delay = 50;
    trans[0].nwords = buff_size;
    trans[0].rxbuffer = in_buff;
    trans[0].txbuffer = buffer;

    board_gpio_write(MAX7219_SPI_CS, 0);

    ret = ioctl(fd, SPIIOC_TRANSFER, (unsigned long)(uintptr_t)&seq);
    board_gpio_write(MAX7219_SPI_CS, 1);

    return ret;
}

void matrix_commit(led_matrix_t *matrix){
    for(uint8_t col = 0; col < matrix->number_of_devices; col++){
        send_byte_device(col/8, col % 8 + 1, matrix->cols[col], matrix);
    }
}
