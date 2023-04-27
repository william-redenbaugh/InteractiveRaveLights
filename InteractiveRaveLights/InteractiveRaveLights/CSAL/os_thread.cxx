#include "os_thread.h"
#include "pthread.h"
#include "errno.h"

os_thread_id_t os_add_thread(thread_func_t p, void * arg, int stack_size, void *stack){
    pthread_t t;
    pthread_create(&t, NULL, (pthread_startroutine_t)p, arg);

    return(os_thread_id_t)t;
}

os_thread_id_t os_kill_thread(os_thread_id_t target_thread_id){
    int ret = pthread_kill((pthread_t)target_thread_id, SA_SIGINFO);

    if(ret == ESRCH){
        return -1;
    }

    return target_thread_id;
}

void os_kill_self_thread(void){
    os_thread_id_t id = os_current_id();

    os_kill_thread(id);
}