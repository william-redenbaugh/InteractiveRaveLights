#include "ipc.h"

ipc_message_header_t deserialize_message_header(uint8_t *buffer, size_t len){
    ipc_message_header_t msg;
    msg.message_len = -1;

    if(len != IPC_MESSAGE_HANDLER_SIZE){
        return msg;
    }

    msg.message_len =
        ((buffer[0] & 0xFF) <<  0) |
        ((buffer[1] & 0xFF) <<  8) |
        ((buffer[2] & 0xFF) << 16) |
        ((int32_t) (buffer[3] & 0xFF) << 24);

    msg.message_id =
        ((buffer[4] & 0xFF) <<  0) |
        ((buffer[5] & 0xFF) <<  8) |
        ((buffer[6] & 0xFF) << 16) |
        ((int32_t) (buffer[7] & 0xFF) << 24);

    return msg;
}

bool serialize_message_header(ipc_message_header_t msg, uint8_t *buffer, size_t len){

    if(len != IPC_MESSAGE_HANDLER_SIZE){
        return false;
    }

    buffer[0] = msg.message_len;
    buffer[1] = msg.message_len >> 8;
    buffer[2] = msg.message_len >> 16;
    buffer[3] = msg.message_len >> 24;

    buffer[4] = msg.message_id;
    buffer[5] = msg.message_id >> 8;
    buffer[6] = msg.message_id >> 16;
    buffer[7] = msg.message_id >> 24;

    return true;
}