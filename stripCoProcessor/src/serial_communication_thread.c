#include "serial_communication_thread.h"
#include "FreeRTOS.h"
#include "hardware/uart.h"
#include "led_matrix_generic.h"
#include "pico/stdlib.h"
#include "stdio.h"
#include "malloc.h"

#define UART_ID uart0
#define BAUD_RATE 115200

// We are using pins 0 and 1, but see the GPIO function select table in the
// datasheet for information on which other pins can be used.
#define UART_TX_PIN 0
#define UART_RX_PIN 1

uint8_t input_buffer[INPUT_BUFFER_SIZE];

void serial_communication_setup(void *params)
{
    // Set up our UART with the required speed.
    uart_init(UART_ID, BAUD_RATE);

    // Set the TX and RX pins by using the function select on the GPIO
    // Set datasheet for more information on function select
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);
}

void uart_read_blocking_threadsafe(uart_inst_t *uart, uint8_t *dst, size_t len)
{
    for (size_t i = 0; i < len; ++i)
    {
        while (!uart_is_readable(uart))
            taskYIELD();
        *dst++ = (uint8_t)uart_get_hw(uart)->dr;
    }
}

UpdateStripStruct_t *new_strip_struct(StripGeneric_t *strip, int num_leds)
{
    UpdateStripStruct_t *new_strip = pico_malloc(sizeof(UpdateStripStruct_t));
    new_strip->mutex = xSemaphoreCreateMutex();
    xEventGroupClearBits(new_strip->new_data_group, BIT_0);

    // STRIP BUFFER
    new_strip->buffer = pico_malloc(sizeof(uint8_t) * num_leds * NUM_COLS_PER_PIXEL);
}

/**
 * @brief Generic thread that
 */
void update_strip_thread(void *params)
{
    // Get handler
    UpdateStripStruct_t *strip_handler = (UpdateStripStruct_t *)params;

    // Clear strip
    for (int n = 0; n < strip_handler->num_leds; n++)
    {
        strip_handler->strip->strip_set(n, 0, 0, 0);
    }
    strip_handler->strip->strip_update();

    // Now wait for data from serial communication thread
    for (;;)
    {
        xEventGroupWaitBits(strip_handler->new_data_group,
                            BIT_0,
                            pdTRUE,
                            pdFALSE,
                            portMAX_DELAY);

        xSemaphoreTake(strip_handler->mutex, portMAX_DELAY);
        for (int n = 0; n < strip_handler->num_leds; n++)
        {
            size_t pos = n * 3;
            uint8_t r = strip_handler->buffer[pos];
            uint8_t g = strip_handler->buffer[pos + 1];
            uint8_t b = strip_handler->buffer[pos + 2];
            strip_handler->strip->strip_set(n, r, g, b);
        }
        strip_handler->strip->strip_update();
    }
}

void serial_commuincation_thread(void *params)
{
    for (;;)
    {
        uart_read_blocking_threadsafe(UART_ID, input_buffer, INPUT_BUFFER_SIZE);
        printf("nothing should be seen here\n");
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}
