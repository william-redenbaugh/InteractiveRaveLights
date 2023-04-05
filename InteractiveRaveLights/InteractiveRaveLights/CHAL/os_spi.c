#include "os_spi.h"
#include "CSAL/os_error.h"

#include <nuttx/config.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <debug.h>
#include <nuttx/spi/spi_transfer.h>

os_spi_t spi_zero = {
    .fd = 0, 
    .xfer_speed = 1000000, 
    .bus = 0, 
    .spi_mode = 1
};

os_spi_t spi_three= {
    .fd = 0, 
    .xfer_speed = 1000000, 
    .bus = 3,
    .spi_mode = 1
};

os_spi_t spi_four = {
    .fd = 0, 
    .xfer_speed = 1000000, 
    .bus = 4,
    .spi_mode = 1
};

os_spi_t spi_five = {
    .fd = 0, 
    .xfer_speed = 1000000, 
    .bus = 5,
    .spi_mode = 1
};


int os_spi_begin(os_spi_t *spi){
    if(spi == NULL)
        return OS_RET_NULL_PTR;

    switch(spi->fd){
        case 0:
            spi->fd = open("/dev/spi0", O_WRONLY);
        break;
        case 3:
            spi->fd = open("/dev/spi3", O_WRONLY);
        break;
        case 4:
            spi->fd = open("/dev/spi4", O_WRONLY);
        break;
        case 5:
            spi->fd = open("/dev/spi5", O_WRONLY);
        break;
    }

    if(spi->fd < 0){
        return OS_RET_IO_ERROR;
    }

    return OS_RET_OK;
}

int os_spi_end(os_spi_t *spi){
    if(spi == NULL)
        return OS_RET_NULL_PTR;

    if (spi->fd <= 0){
        return OS_RET_INT_ERR;
    }
    
    close(spi->fd);

    return OS_RET_OK;    
}

int os_spi_setbus(os_spi_t *spi, uint32_t freq_hz){
    if(spi == NULL)
        return OS_RET_NULL_PTR;

    spi->xfer_speed = freq_hz;

    return OS_RET_OK;
}

int os_spi_transfer(os_spi_t *spi, uint8_t *rx, uint8_t *tx, size_t size){
    if(spi == NULL)
        return OS_RET_NULL_PTR;

    struct spi_trans_s    trans[1];
    struct spi_sequence_s seq;

    seq.dev       = 0;
    seq.mode      = spi->spi_mode;
    seq.nbits     = 8;
    seq.frequency = spi->xfer_speed;
    seq.ntrans    = 1;
    seq.trans     = trans;

    trans[0].deselect = false;
    trans[0].delay    = 10;
    trans[0].nwords   = sizeof(tx);
    trans[0].txbuffer = tx;
    trans[0].rxbuffer = rx;

    int ret = ioctl(spi->fd, SPIIOC_TRANSFER, (unsigned long)((uintptr_t)&seq));
    
    if(ret < 0){
        return OS_RET_IO_ERROR;
    }
    
    return OS_RET_OK;

}

int os_spi_send(os_spi_t *spi, uint8_t *buf, size_t size){
    if(spi == NULL)
        return OS_RET_NULL_PTR;
        
    struct spi_trans_s    trans[1];
    struct spi_sequence_s seq;
    uint8_t *rx;

    seq.dev       = 0;
    seq.mode      = spi->spi_mode;
    seq.nbits     = 8;
    seq.frequency = spi->xfer_speed;
    seq.ntrans    = 1;
    seq.trans     = trans;

    trans[0].deselect = false;
    trans[0].delay    = 10;
    trans[0].nwords   = sizeof(buf);
    trans[0].txbuffer = buf;
    trans[0].rxbuffer = rx;

    int ret = ioctl(spi->fd, SPIIOC_TRANSFER, (unsigned long)((uintptr_t)&seq));
    if(ret < 0){
        return OS_RET_IO_ERROR;
    }

    return OS_RET_OK;
}

int os_spi_recieve(os_spi_t *spi, uint8_t *buf, size_t size){
    if(spi == NULL)
        return OS_RET_NULL_PTR;

    struct spi_trans_s    trans[1];
    struct spi_sequence_s seq;
    uint8_t *tx;

    seq.dev       = 0;
    seq.mode      = spi->spi_mode;
    seq.nbits     = 8;
    seq.frequency = spi->xfer_speed;
    seq.ntrans    = 1;
    seq.trans     = trans;

    trans[0].deselect = false;
    trans[0].delay    = 10;
    trans[0].nwords   = sizeof(buf);
    trans[0].txbuffer = tx;
    trans[0].rxbuffer = buf;

    int ret = ioctl(spi->fd, SPIIOC_TRANSFER, (unsigned long)((uintptr_t)&seq));
    if(ret < 0){
        return OS_RET_IO_ERROR;
    }
    
    return OS_RET_OK;
}