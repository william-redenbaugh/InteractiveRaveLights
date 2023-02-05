#include "ipc_thread.h"
#include "pthread.h"
#include "tiny-json/tiny-json.h"
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sched.h>
#include <errno.h>
#include "ipc.h"
#include "termios.h"
#include "stdint.h"

int uart_fd = -1;
uint8_t *content_buffer_arr;

void uart_ipc_thread(void *params)
{
    uart_fd = open("/dev/ttyS2", O_RDWR);
    // String buffer array for us to
    content_buffer_arr = malloc(sizeof(uint8_t) * BUFF_ARR_MAX_SIZE);

    if (uart_fd < 0)
    {
        printf("UART had issues initializing: %d\n", uart_fd);
    }

    for (;;)
    {
        ipc_message_header_t header = ipc_get_header(uart_fd);
        // If string is larger than buffer we can't accept the string
        if (header.message_len > BUFF_ARR_MAX_SIZE)
        {
            printf("Requested Packet Buffer Overflow detection\n");
            // Let message pass
            // Wait 1 second
            usleep(1000000);
            // Flush buffer
            tcflush(uart_fd, TCIOFLUSH);
            // Send error
        }
        else
        {
            // Get
            int ret = read(uart_fd, content_buffer_arr, header.message_len);
        }
    }
}