#include <stdio.h>
#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "hardware/uart.h"
#include "threads_init.h"

/**
 * @brief Simply Entry Function intializes peripherials
 * @note Just starts incoming threads and scheduler
 */
int main(void)
{
    stdio_init_all();
    vTaskStartScheduler();
    threads_list_init();
    vTaskSuspend(NULL);
}