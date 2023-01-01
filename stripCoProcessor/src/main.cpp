#include <stdio.h>
#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "hardware/uart.h"
#include "threads_init.h"
#include "led_strip_management.h"

/**
 * @brief Simply Entry Function intializes peripherials
 * @note Just starts incoming threads and scheduler
 */
int main(void)
{
    // Initialized standardized rpi hardware
    stdio_init_all();

    // Start Scheduler
    vTaskStartScheduler();

    // Initialized
    threads_list_init();
    vTaskSuspend(NULL);
}