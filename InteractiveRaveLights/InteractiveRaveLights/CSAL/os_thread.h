#ifndef _OS_THREAD_H
#define _OS_THREAD_H

#include "stdint.h"
#include "stdbool.h"
#include "pthread.h"

/*!
* @brief  Thread id value
* @note
*/
typedef pthread_t os_thread_id_t;

/*!
* @brief Redeclaration of thread function
* @note Holds pointer to begining of thread function subroutine. Holds register space for void pointer
*/
typedef void (*thread_func_t)(void*);

/*!
* @brief Adds a thread to Will-OS Kernel
* @note Paralelism at it's finest!
* @param will_os_thread_func_t thread(pointer to thread function call begining of program counter)
* @param void *arg(pointer arguement to parameters for thread)
* @param void *stack(pointer to begining of thread stack)
* @param int stack_size(size of the allocated threadstack)
* @returns none
*/
os_thread_id_t os_add_thread(thread_func_t p, void * arg, int stack_size, void *stack);

/*!
* @brief Sets the state of a thread to suspended.
* @brief If thread doesn't exist, then
* @param Which thread are we trying to get our state for
* @returns will_thread_state_t
*/
os_thread_id_t os_suspend_thread(os_thread_id_t target_thread_id);

/*!
* @brief Sets the state of a thread to resumed.
* @brief If thread doesn't exist or hasn't been run before, then
* @param Which thread are we trying to get our state for
* @returns will_thread_state_t
*/
os_thread_id_t os_resume_thread(os_thread_id_t target_thread_id);

/*!
* @brief Sets the state of a thread to dead
* @brief If thread doesn't exist, then
* @param Which thread are we trying to get our state for
* @returns will_thread_state_t
*/
os_thread_id_t os_kill_thread(os_thread_id_t target_thread_id);

/*!
* @returns The current thread's ID.
*/
#define os_current_id() pthread_self()

/*!
 * @brief Kills and closes out current thread
*/
void os_kill_self_thread(void);

/*!
* @brief Sleeps the thread through a hypervisor call.
* @note Sleeps the thread for the alloted time, and wakes up once the thread is ready
* @param int milliseconds since last system tick
* @returns none
*/
#define os_thread_sleep_ms(millisecond) usleep(1000000 * millisecond)

/*!
* @brief Sleeps the thread through a hypervisor call.
* @note Sleeps the thread for the alloted time, and wakes up once the thread is ready
* @param int seconds since last system tick
* @returns none
*/
#define os_thread_sleep_s(s) os_thread_sleep_ms(s * 1000)

/*!
*   @brief allows our program to "yield" out of current subroutine
*   @note Call's hypervisor command to look into something else.
*/
#define _os_yield() pthread_yield()
#endif