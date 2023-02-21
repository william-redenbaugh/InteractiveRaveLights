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
#include "ipc_message_publishqueue.h"
#include "ipc_message_subscribequeue.h"

int uart_fd = -1;
uint8_t *content_buffer_arr;

void uart_ipc_publish_init(void *params)
{
    init_ipc_message_queue();
}

void uart_ipc_publish_thread(void *params)
{
    for (;;)
    {
        // Wait until we can consume the entire node
        ipc_message_node_t event_node = ipc_block_consume_new_event();

        // ipc_message_callback_status
        ipc_message_ret_t callback_ret;
        callback_ret.ipc_status = IPC_MESSAGE_COMPLETE_SUCCESS;

        // Writes the message header to UART first!
        int write_size = write(uart_fd, (void *)&event_node.message_header, sizeof(ipc_message_header_t));

        // If we couldn't write complete header, say we failed to publish!
        if (write_size != sizeof(ipc_message_header_t))
        {
            callback_ret.ipc_status = IPC_MESSAGE_COMPLETE_FAIL;
        }

        // Write message contents after
        if (event_node.message_header.message_len > 0 | event_node.buffer_ptr != NULL)
        {
            write_size = write(uart_fd, (void *)&event_node.buffer_ptr, event_node.message_header.message_len);
            // If we couldn't write entire message, say we failed to publish!
            if (write_size != sizeof(ipc_message_header_t))
            {
                callback_ret.ipc_status = IPC_MESSAGE_COMPLETE_FAIL;
            }
        }

        if (event_node.callback_func != NULL)
            // Any cleanup needed for the published event!
            event_node.callback_func(callback_ret);
    }
}

void uart_ipc_consume_thread_init(void *params)
{
    uart_fd = open("/dev/ttyS2", O_RDWR);
    // String buffer array for us to
    content_buffer_arr = malloc(sizeof(uint8_t) * BUFF_ARR_MAX_SIZE);

    if (uart_fd < 0)
    {
        printf("UART had issues initializing: %d\n", uart_fd);
    }
}

void uart_ipc_consume_thread(void *params)
{

    for (;;)
    {
        ipc_message_header_t header = ipc_get_header_from_uart(uart_fd);
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
            if (ret != header.message_len)
            {
                printf("Failiure to get entire message");
                // Let message pass
                // Wait 1 second
                usleep(1000000);
                // Flush buffer
                tcflush(uart_fd, TCIOFLUSH);
                // Send error
            }
            else
            {
                // Will run all callbacks related to a specific
                // Enumerated message!
                ipc_run_all_sub_cb(header, content_buffer_arr);
                printf("parse message!");
            }
        }
    }
}