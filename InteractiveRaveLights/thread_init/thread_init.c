#include "threads_init.h"
#include "threads_list.h"
#include "pthread.h"

void threads_list_init(void)
{
    task_init_descriptor_t threads[NUM_THREADS] = THREAD_LIST;

    // Launch each task!
    for (int n = 0; n < NUM_THREADS; n++)
    {
        pthread_create(NULL, NULL, threads[n].task_fun, NULL);
    }
}