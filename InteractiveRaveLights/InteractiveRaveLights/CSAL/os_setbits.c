#include "os_setbits.h"

int os_setbits_init(os_setbits_t *mod){
    mod->bits = 0x00000000;
    sem_init(&mod->bit_sem, 0, 0);
    sem_setprotocol(&mod->bit_sem, SEM_PRIO_NONE);

    return OS_RET_OK;
}

int os_setbits_signal(os_setbits_t *mod, uint8_t bit){
    if(bit >= 32)
        return OS_RET_INVALID_PARAM;

    mod->bits |= (1 << bit);
        
    if(mod->sem_wait){
        mod->sem_wait = false;
        sem_post(&mod->bit_sem);
    }
    
    return OS_RET_OK;
}

int os_clearbits(os_setbits_t * mod, uint8_t bit){
    if(bit >= 32)
        return OS_RET_INVALID_PARAM;

    mod->bits &= (0 << bit);
        
    if(mod->sem_wait){
        mod->sem_wait = false;
        sem_post(&mod->bit_sem);
    }

    return OS_RET_OK;
}

int os_waitbits(os_setbits_t *mod, uint8_t bit, uint32_t timeout_ms){
    if(bit >= 32)
        return OS_RET_INVALID_PARAM;

    if(mod->bits && (1 << bit)){
        return OS_RET_OK;
    }

    struct timespec ts;
    ts.tv_sec = timeout_ms / 1000;
    ts.tv_nsec = (timeout_ms % 1000) * 1000000;
    
    while(mod->bits && (1 << bit) == 0){
        mod->sem_wait = true;
        int ret = sem_timedwait(&mod->bit_sem, &ts);
        int n = posix_os_ret_error_conv(ret);

        switch(n){
            case OS_RET_TIMEOUT:
                return OS_RET_TIMEOUT;
            default:
                break;
        }
    }
    return OS_RET_OK;
}

int os_waitbits_indefinite(os_setbits_t *mod, uint8_t bit, uint32_t timeout_ms){
    if(bit >= 32)
        return OS_RET_INVALID_PARAM;
    
    if(mod->bits && (1 << bit)){
        return OS_RET_OK;
    }
    while(mod->bits && (1 << bit) == 0){
        mod->sem_wait = true;
        while(mod->sem_wait){
            int ret = sem_wait(&mod->bit_sem);
        }
    }

    return OS_RET_OK;
}