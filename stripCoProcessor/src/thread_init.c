#include "threads_init.h"
#include "threads_list.h"

void threads_list_init(void)
{
    task_init_descriptor_t threads[NUM_THREADS] = THREAD_LIST;
    for (int n = 0; n < NUM_THREADS; n++)
    {
        threads[n].task_init_fun(NULL);
    }

    // Launch each task!
    for (int n = 0; n < NUM_THREADS; n++)
    {
        xTaskCreate(threads[n].task_fun, threads[n].task_name, threads[n].stack_size, threads[n].param, threads[n].priority, threads[n].handle);
    }
}
