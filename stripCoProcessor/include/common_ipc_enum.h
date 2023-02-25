#ifndef _COMMON_IPC_ENUM_H
#define _COMMON_IPC_ENUM_H

/**
 * @brief COMMON IPC ENUM MESSAGES
 */
typedef enum ipc_type_enum
{
    /**
     * @brief Message used to let other side know we acked their message
     */
    IPC_TYPE_ACK = 0,
    IPC_TYPE_TEST = 1,
} ipc_type_enum_t;

/**
 * @brief Number of different ipc enum messages
 */
#define IPC_TYPE_ENUM_LEN 2

#endif