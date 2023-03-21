#ifndef _IPC_THREAD_H
#define _IPC_THREAD_H
/**
 * Module explaination!
 * So this is the UART IPC module!
 *
 * There are two separate threads for the IPC transaction: the Publish and Consume threads
 *
 * The Publish thread sits and waits for messages to be added to a queue
 * Then will go through each message, process the data, implement the forward error correction(eventually)
 * And then push the data through the uart interface
 *
 * The consume thread will wait for any data through the UART bus
 * Manage error correction, then any callbacks that are subscribed can then runs those callbacks!
 */

/**
 * @brief Initialization module for the publish module for the UART IPC
 * @note See top for more information
 */
void uart_ipc_publish_init(void *params);

/**
 * @brief Publish thread! See note at top of file for more information
 */
void uart_ipc_publish_thread(void *params);

/**
 * @brief Thread intialization for the consumption thread
 */
void uart_ipc_consume_thread_init(void *params);
/**
 * @brief Consume thread! Get's UART events from the bus
 * And runs callbacks based off those
 * Will also publish events
 */
void uart_ipc_consume_thread(void *params);

#endif