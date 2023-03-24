#include "ipc.h"
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sched.h>
#include <errno.h>
#include "ipc.h"
#include "termios.h"
#include "stdint.h"

ipc_message_header_t ipc_get_header_from_uart(int uart_fd)
{
    // Get message header bytes
    uint8_t header_arr[IPC_MESSAGE_HANDLER_SIZE];
    int total_bytes_uploaded = 0;
    ipc_message_header_t header;
    header.message_len = -1;
    header.message_id = -1;

    while(total_bytes_uploaded < 9){
        int ret = read(uart_fd, &header_arr[total_bytes_uploaded], 9 - total_bytes_uploaded);
        total_bytes_uploaded += ret;
        usleep(1000);
    }
    // Deserialize
    header = deserialize_message_header(header_arr, sizeof(header_arr));

    return header;
}

ipc_message_header_t deserialize_message_header(uint8_t *buffer, size_t len)
{
    ipc_message_header_t msg;
    msg.message_len = -1;

    msg.message_len =
        (int32_t)(buffer[0]) |
        (int32_t)(buffer[1] << 8) |
        (int32_t)(buffer[2] << 16) |
        (int32_t)(buffer[3] << 24);

    msg.message_id =
        (int32_t)(buffer[4]) |
        (int32_t)((buffer[5] ) << 8) |
        (int32_t)((buffer[6] ) << 16) |
        (int32_t)(buffer[7] << 24);

    // What type of message are we sending/receiving
    msg.message_type_enum = buffer[8];

    return msg;
}

bool serialize_message_header(ipc_message_header_t msg, uint8_t *buffer, size_t len)
{

    if (len != IPC_MESSAGE_HANDLER_SIZE)
    {
        return false;
    }

    buffer[0] = (uint8_t)msg.message_len;
    buffer[1] = (uint8_t)(msg.message_len >> 8);
    buffer[2] = (uint8_t)(msg.message_len >> 16);
    buffer[3] = (uint8_t)(msg.message_len >> 24);

    buffer[4] = (uint8_t)msg.message_id;
    buffer[5] = (uint8_t)msg.message_id >> 8;
    buffer[6] = (uint8_t)msg.message_id >> 16;
    buffer[7] = (uint8_t)msg.message_id >> 24;

    buffer[8] = msg.message_type_enum;
    return true;
}

bool ipc_send_error_message(int fd)
{
    char error_str[] = "{\" return_status \": \" error \"}";
}