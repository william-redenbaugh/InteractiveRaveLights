#include <stdio.h>
#include "pico/stdlib.h"
#include "FreeRTOS-Kernel/include/FreeRTOS.h"
#include "hardware/uart.h"

typedef struct RgbColor{
    uint8_t r;
    uint8_t g;
    uint8_t b;
}rgb_color_t;

#define UART_ID uart0
#define BAUD_RATE 115200

// We are using pins 0 and 1, but see the GPIO function select table in the
// datasheet for information on which other pins can be used.
#define UART_TX_PIN 0
#define UART_RX_PIN 1

#define NUM_PIXELS_PER_STRIP 96
#define NUM_COLS_PER_PIXEL 3
#define NUM_STRIPS 3
#define INPUT_BUFFER_SIZE NUM_PIXELS_PER_STRIP * NUM_COLS_PER_PIXEL * NUM_STRIPS
rgb_color_t input_buffer[INPUT_BUFFER_SIZE];

rgb_color_t strip_one_buff[NUM_PIXELS_PER_STRIP];

void uart_read_blocking_threadsafe(uart_inst_t *uart, uint8_t *dst, size_t len) {
    for (size_t i = 0; i < len; ++i) {
        while (!uart_is_readable(uart))
            taskYIELD();
        *dst++ = (uint8_t) uart_get_hw(uart)->dr;
    }
}

void uart_rx_thread(void *params){
    // Set up our UART with the required speed.
    uart_init(UART_ID, BAUD_RATE);

    // Set the TX and RX pins by using the function select on the GPIO
    // Set datasheet for more information on function select
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);

    for(;;){
        uart_read_blocking_threadsafe(UART_ID, input_buffer, INPUT_BUFFER_SIZE);
        printf("nothing should be seen here\n");

        for(int)
        vTaskDelay(10/portTICK_PERIOD_MS);
    }
}

void strip_thread_one(void *params){

}

void strip_thread_two(void *params){

}

void strip_thread_three(void *params){

}

void logging_thread(void *params){
    for(;;){
        vTaskDelay(1000/portTICK_PERIOD_MS);
    }
}

int main(void)
{
    stdio_init_all();
    xTaskCreate(uart_rx_thread, "UART communication thread", 2048, NULL, 1, NULL);
    xTaskCreate(logging_thread, "Test printing thread", 768, NULL, 1, NULL);
    vTaskStartScheduler();
    vTaskSuspend(NULL);
}
