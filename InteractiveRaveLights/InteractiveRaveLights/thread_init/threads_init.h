#ifndef THREADS_INIT_H
#define THREADS_INIT_H
#include "sys/_stdint.h"
#include "pthread.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * @brief Structure that manages task initialization.
 * @note Designed to help intiailize lists of tasks all at once.
 */
typedef struct {
    void(*task_fun)(void *task_params);
    void(*task_init_fun)(void *params);
    const char *task_name;
    void *param;
}task_init_descriptor_t;

/**
 * @brief Initialize all the threads that we put into our list.
 *
 */
extern void threads_list_init(void);

#ifdef __cplusplus
}
#endif
#endif