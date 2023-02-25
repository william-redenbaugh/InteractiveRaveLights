#include "threads_init.h"
#include "threads_list.h"
void threads_list_init(void)
{
    task_init_descriptor_t threads[NUM_THREADS] = THREAD_LIST;
    for (int n = 0; n < NUM_THREADS; n++)
    {
        if(threads[n].task_init_fun!= NULL)
            threads[n].task_init_fun(NULL);
    }

    // Launch each task!
    for (int n = 0; n < NUM_THREADS; n++)
    {
        if(threads[n].task_fun != NULL){
            // New block of memory from heap for stack pointer
            uint8_t *stack_ptr = (uint8_t*)malloc(sizeof(threads[n].stack_size));
            os_add_thread((thread_func_t)threads[n].task_fun, threads[n].param, threads[n].stack_size, stack_ptr);
        }
    }
}
