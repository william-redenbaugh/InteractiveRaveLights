#include "threads_init.h"
#include "threads_list.h"
#include "pthread.h"

extern "C" void threads_list_init(void)
{
    task_init_descriptor_t threads[NUM_THREADS] = THREAD_LIST;

    // Run the initialization function for each task!
    // Takes care of race conditions before they happen
    for(int n = 0; n < NUM_THREADS; n++){
        printf("Launching Task Init Function: %s\n", threads[n].task_name);
        threads[n].task_init_fun(NULL);
    }

    // Launch each task!
    for (int n = 0; n < NUM_THREADS; n++)
    {
        printf("Launching Task: %s\n", threads[n].task_name);
        pthread_create(NULL, NULL, (pthread_startroutine_t)threads[n].task_fun, NULL);
    }
}