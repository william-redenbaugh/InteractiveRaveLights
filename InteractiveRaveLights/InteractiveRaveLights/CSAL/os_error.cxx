#include "os_error.h"
#include "errno.h"

int posix_os_ret_error_conv(int ret){
    switch(ret){
    case EAGAIN:
        return OS_RET_NO_MORE_RESOURCES;
    
    case ENOMEM:
        return OS_RET_LOW_MEM_ERROR;

    case EBUSY:
        return OS_RET_TIMEOUT;

    case EINVAL:
        return OS_RET_INVALID_PARAM;

    case EDEADLK:
        return OS_RET_DEADLOCK;

    case EPERM:
        return OS_RET_NOT_OWNED;
    
    case ENOSYS: 
        return OS_RET_UNSUPPORTED_FEATURES;
        
    case 0:
        return OS_RET_OK;
        
    default:
        return OS_RET_UNDEFINED_ERROR;
    }
}