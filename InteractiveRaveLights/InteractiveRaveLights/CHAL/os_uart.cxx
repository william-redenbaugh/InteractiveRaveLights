#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sched.h>
#include <errno.h>
#include "sys/ioctl.h"
#include "termios.h"
#include "stdint.h"

#include "os_uart.h"
#include "CSAL/os_error.h"

int os_uart_begin(os_uart_t *uart){
    if(uart == NULL)
        return OS_RET_NULL_PTR;   

    switch(uart->bus){
        case(0):
            uart->fd = open("/dev/ttyS0", O_RDWR);
        case(2):
            uart->fd = open("dev/ttyS2", O_RDWR);
        default:
            uart->fd = -1;
            return OS_RET_INVALID_PARAM;
    } 

    struct termios tio;
    /* tty: setup parameters */
    tio.c_cflag |= CREAD;   /* Enable receive */
    tio.c_cflag |= CLOCAL;  /* Local line, no modem control */
    tio.c_cflag &= ~CSIZE;  /* Clean the bits */
    tio.c_cflag |= CS8;     /* Data bit 8bit */
    tio.c_cflag &= ~CSTOPB; /* Stop bit 1bit */
    tio.c_cflag &= ~PARENB; /* Paritiy none */

    tio.c_cc[VMIN] = 1;
    tio.c_cc[VTIME] = 5;
    cfsetspeed(&tio, uart->baud);

    if (tcsetattr(uart->fd, TCSANOW, &tio)){
        printf("Error setting uart attributes\n");
    }
    else{
        printf("Success in setting UART attributes\n");
    }
    const int bits = TIOCM_RTS;
    int ret = ioctl(uart->fd, TIOCMBIS, (unsigned long)&bits);

    if(ret < 0){
        return OS_RET_INT_ERR;
    }

    return OS_RET_OK;
}

int os_uart_end(os_uart_t *uart){
    if(uart == NULL)
        return OS_RET_NULL_PTR;
}

int os_uart_setbus(os_uart_t *uart, uint32_t freq_baud){
    if(uart == NULL)
        return OS_RET_NULL_PTR;

    uart->baud = freq_baud;

    struct termios tio;
    /* tty: setup parameters */
    tio.c_cflag |= CREAD;   /* Enable receive */
    tio.c_cflag |= CLOCAL;  /* Local line, no modem control */
    tio.c_cflag &= ~CSIZE;  /* Clean the bits */
    tio.c_cflag |= CS8;     /* Data bit 8bit */
    tio.c_cflag &= ~CSTOPB; /* Stop bit 1bit */
    tio.c_cflag &= ~PARENB; /* Paritiy none */

    tio.c_cc[VMIN] = 1;
    tio.c_cc[VTIME] = 5;
    cfsetspeed(&tio, uart->baud);

    if (tcsetattr(uart->fd, TCSANOW, &tio)){
        printf("Error setting uart attributes\n");
    }
    else{
        printf("Success in setting UART attributes\n");
    }
    const int bits = TIOCM_RTS;
    int ret = ioctl(uart->fd, TIOCMBIS, (unsigned long)&bits);
}

int os_uart_transfer(os_uart_t *uart, uint8_t *rx, uint8_t*tx, size_t size){
    if(uart == NULL)
        return OS_RET_NULL_PTR;

    // Consecutive read and write operation...
    write(uart->fd, tx, size);
    read(uart->fd, rx, size);
    return OS_RET_OK;
}

int os_uart_send(os_uart_t *uart, uint8_t *buf, size_t size){
    if(uart == NULL)
        return OS_RET_NULL_PTR;

    write(uart->fd, buf, size);
}

int os_uart_recieve(os_uart_t *uart, uint8_t *buf, size_t size){
    if(uart == NULL)
        return OS_RET_NULL_PTR;

    read(uart->fd, buf, size);
}