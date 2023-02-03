#ifndef _UART_IPC_H
#define _UART_IPC_H

#include "stdint.h"
#include "strings.h"
#include "stdbool.h"

/**
 * @brief Message header before we get the actual JSON message so we known the length of the string
*/
#define IPC_MESSAGE_HANDLER_SIZE 8 * sizeof(uint8_t)
typedef struct ipc_message_header{
    int32_t message_len;
    int32_t message_id;

}ipc_message_header_t;

/**
 * @brief Allows us to easily deserialize a message
 * @param uint8_t *buffer pointer to data we are deserializing
 * @param size_t size of buffer, make sure we aren't doing an overwrite
 * @return ipc_message_header_t returned message header for message unpacking
 * @note Will return a message_len of -1 if deserializating fails
*/
ipc_message_header_t deserialize_message_header(uint8_t *buffer, size_t len);

/**
 * @brief Allows us to easily serialize a message
 * @param ipc_message_header_t message we want to serialize
 * @param uint8_t *buffer pointer to the buffer we want to serialize into
 * @param size_t size of buffer we want to serialize into
 * @return bool whether or not we were able to serialize correctly
*/
bool serialize_message_header(ipc_message_header_t msg, uint8_t *buffer, size_t len);
#endif