#ifndef THREADS_INIT_H
#define THREADS_INIT_H
#include "sys/_stdint.h"
#include "pthread.h"

/**
 * @brief Structure that manages task initialization.
 * @note Designed to help intiailize lists of tasks all at once.
 */
typedef struct {
    void(*task_fun)(void *task_params);
    const char *task_name;
    void *param;
}task_init_descriptor_t;

/**
 * @brief Initialize all the threads that we put into our list.
 *
 */
void threads_list_init(void);

#endif