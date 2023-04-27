#include "os_i2c.h"
#include "CSAL/os_error.h"

#include <nuttx/config.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <debug.h>
#include <nuttx/i2c/i2c_master.h>

int os_i2c_begin(os_i2c_t *i2c){
    if(i2c == NULL)
        return OS_RET_NULL_PTR;

    i2c->fd =  open("/dev/i2c0", O_WRONLY);
    i2c->speed = 400000;
    if (i2c->fd< 0){
        return OS_RET_IO_ERROR;
    }

    return OS_RET_OK;

}

int os_i2c_end(os_i2c_t *i2c){
    if(i2c == NULL)
        return OS_RET_NULL_PTR;

    if (i2c->fd <= 0){
        return OS_RET_INT_ERR;
    }
    
    close(i2c->fd);
    return OS_RET_OK;
}

int os_i2c_setbus(os_i2c_t *i2c, uint32_t freq_hz){
    if(i2c == NULL)
        return OS_RET_NULL_PTR;

    i2c->speed = freq_hz;
    return OS_RET_OK;
}

int os_i2c_send(os_i2c_t *i2c, uint8_t addr, uint8_t *buf, size_t size){
    if(i2c == NULL)
        return OS_RET_NULL_PTR;

    struct i2c_msg_s msg;
    struct i2c_transfer_s xfer; 
    
    msg.frequency = i2c->speed;
    msg.addr      = addr;
    msg.buffer    = buf;
    msg.length    = size;
    msg.flags     = 0;

    xfer.msgv = &msg;

    xfer.msgc = 1;

    int ret = ioctl(i2c->fd, I2CIOC_TRANSFER, (unsigned long)(uintptr_t)&xfer);
  
    if(ret < 0)
        return OS_RET_IO_ERROR;

    return OS_RET_OK;
}

int os_i2c_recieve(os_i2c_t *i2c, uint8_t addr, uint8_t *buf, size_t size){
    if(i2c == NULL)
        return OS_RET_NULL_PTR;


    struct i2c_msg_s msg;
    struct i2c_transfer_s xfer; 
    
    msg.frequency = i2c->speed;
    msg.addr      = addr;
    msg.buffer    = buf;
    msg.length    = size;
    msg.flags     = I2C_M_READ;

    xfer.msgv = &msg;

    xfer.msgc = 1;

    int ret = ioctl(i2c->fd, I2CIOC_TRANSFER, (unsigned long)(uintptr_t)&xfer);
  
    if(ret < 0)
        return OS_RET_IO_ERROR;

    return OS_RET_OK;
}