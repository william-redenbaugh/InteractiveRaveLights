#include "os_thread.h"
#include "pthread.h"
#include "errno.h"

os_thread_id_t os_add_thread(thread_func_t p, void * arg, int stack_size, void *stack){
    pthread_t t;
    pthread_create(NULL, &t, p, arg);

    return(os_thread_id_t)t;
}

/*!
* @brief Sets the state of a thread to dead
* @brief If thread doesn't exist, then
* @param Which thread are we trying to get our state for
* @returns will_thread_state_t
*/
os_thread_id_t os_kill_thread(os_thread_id_t target_thread_id){
    int ret = pthread_cancel((pthread_t)target_thread_id);

    if(ret == ESRCH){
        return -1;
    }

    return target_thread_id;
}