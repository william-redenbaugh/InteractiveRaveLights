#include "os_adc.h"
#include <nuttx/config.h>
#include <stdio.h>
#include "thread_init/threads_init.h"
#include "pthread.h"
#include "CSAL/os_error.h"
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include "pthread.h"
#include <arch/chip/scu.h>
#include <arch/chip/adc.h>
#include "nuttx/wqueue.h"

os_adc_t adc_one = {
    .bus = 1, 
    .num_bits = 16,
    .fifosize = 128
};
os_adc_t adc_two = {
    .bus = 2, 
    .num_bits = 16,
    .fifosize = 128
};
os_adc_t adc_three = {
    .bus = 3, 
    .num_bits = 16,
    .fifosize = 128
};
os_adc_t adc_four = {
    .bus = 4, 
    .num_bits = 16,
    .fifosize = 128
};
os_adc_t adc_five = {
    .bus = 5, 
    .num_bits = 16,
    .fifosize = 128
};
os_adc_t adc_size = {
    .bus = 6, 
    .num_bits = 16,
    .fifosize = 128
};

int os_adc_begin(os_adc_t *adc){
        
    if(adc == NULL){
        return OS_RET_NULL_PTR;
    }

    int errval, errno; 

    if(adc)
    switch(adc->bus){
        case 1:
            adc->fd = open("/dev/hpadc0", O_RDONLY);
        break;
        case 2:
            adc->fd = open("/dev/hpadc1", O_RDONLY);
        break;
        case 3:
            adc->fd = open("/dev/lpadc0", O_RDONLY);
        break;
        case 4:
            adc->fd = open("/dev/lpadc1", O_RDONLY);
        break;
        case 5:
            adc->fd = open("/dev/lpadc2", O_RDONLY);
        break;
        case 6:
            adc->fd = open("/dev/lpadc3", O_RDONLY);
        break;
    }

    if (adc->fd < 0){
        errval = 4;
        return OS_RET_NO_MORE_RESOURCES;
    }

    int ret = ioctl(adc->fd, SCUIOC_SETFIFOMODE, 1);
    if (ret < 0){
        close(adc->fd);
        return OS_RET_LOW_MEM_ERROR;
    }

    ret = ioctl(adc->fd, ANIOC_CXD56_FIFOSIZE, adc->fifosize * 2);
    if (ret < 0){
        close(adc->fd);
        return OS_RET_LOW_MEM_ERROR;
    }

    ret = ioctl(adc->fd, ANIOC_CXD56_START, 0);
    if (ret < 0){
        close(adc->fd);
        return OS_RET_UNDEFINED_ERROR;
    }

    return OS_RET_OK;
}

int os_adc_end(os_adc_t *adc){
        
    if(adc == NULL){
        return OS_RET_NULL_PTR;
    }

    int ret = ioctl(adc->fd, ANIOC_CXD56_STOP, 0);
    if (ret < 0){
        return OS_RET_INT_ERR;
    }

    close(adc->fd);
    return OS_RET_OK;
}

int os_adc_read(os_adc_t *adc, void *value){
    read(adc->fd, value, 2);
    return OS_RET_OK;
}

int os_adc_read_values(os_adc_t *adc, uint32_t size, void *values){

    if(adc == NULL){
        return OS_RET_NULL_PTR;
    }
    read(adc->fd, values, size *2);

    return OS_RET_OK;
}