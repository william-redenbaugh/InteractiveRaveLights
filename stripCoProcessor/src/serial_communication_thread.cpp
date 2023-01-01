#include "serial_communication_thread.h"
#include "FreeRTOS.h"
#include "hardware/uart.h"
#include "led_matrix_generic.h"
#include "pico/stdlib.h"
#include "stdio.h"
#include "pico/malloc.h"
#include "led_strip_management.h"

#define UART_ID uart0
#define BAUD_RATE 115200

// We are using pins 0 and 1, but see the GPIO function select table in the
// datasheet for information on which other pins can be used.
#define UART_TX_PIN 0
#define UART_RX_PIN 1

uint8_t input_buffer[INPUT_BUFFER_SIZE];
UpdateStripStruct_t *strip_handler_list[NUM_STRIPS];
void update_strip_thread(void *params);

UpdateStripStruct_t *new_strip_struct(StripGeneric_t *strip, int num_leds)
{
    UpdateStripStruct_t *new_strip = (UpdateStripStruct_t*)pvPortMalloc(sizeof(UpdateStripStruct_t));
    new_strip->mutex = xSemaphoreCreateMutex();
    xEventGroupClearBits(new_strip->new_data_group, BIT_0);

    // STRIP BUFFER
    new_strip->buffer = (uint8_t*)pvPortMalloc(sizeof(uint8_t) * num_leds * NUM_COLS_PER_PIXEL);
}

void serial_communication_setup(void *params)
{
    // Set up our UART with the required speed.
    uart_init(UART_ID, BAUD_RATE);

    // Set the TX and RX pins by using the function select on the GPIO
    // Set datasheet for more information on function select
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);

    // Get all our generic strips
    // From our strip management module
    StripGeneric_t *strips[NUM_STRIPS];
    strips[0] = get_setup_strip_one();
    strips[1] = get_setup_strip_two();
    strips[2] = get_setup_strip_three();

    // Launch threads that will help handle strip updates
    for (int n = 0; n < NUM_STRIPS; n++){
        strip_handler_list[n] = new_strip_struct(strips[n], NUM_PIXELS_PER_STRIP);
        xTaskCreate(update_strip_thread,
                    "Strip threads",
                    1024,
                    (void *)strip_handler_list[n],
                    1,
                    NULL);
    }
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

/**
 * @brief Generic thread that
 */
void update_strip_thread(void *params)
{
    for (;;){
        printf("Hello  from strip\n");
        vTaskDelay(3000 / portTICK_RATE_MS);
    }
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

/**
 * @brief Thread that will get data from the serial bus and put it into
*/
void serial_commuincation_thread(void *params)
{
    for (;;)
    {
        uart_read_blocking_threadsafe(UART_ID, input_buffer, INPUT_BUFFER_SIZE);
        int k_pos = 0;
        // Increment through strips
        for (int n = 0; n < NUM_STRIPS; n++)
        {
            // Threadsafe copy operation
            xSemaphoreTake(strip_handler_list[n]->mutex, portMAX_DELAY);
            int x = 0;
            // Copy contents into buffer
            for (x; x < NUM_PIXELS_PER_STRIP * NUM_COLS_PER_PIXEL; x++)
            {
                strip_handler_list[n]->buffer[x] = input_buffer[k_pos + x];
            }
            // Done doing thread sensitive copying
            xSemaphoreGive(strip_handler_list[n]->mutex);

            // change pointer of input buffer for next strip
            k_pos += x;

            // Notify strip that it can push data now
            xEventGroupSetBits(strip_handler_list[n]->new_data_group, BIT_0);
        }
    }
}