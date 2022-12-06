#include "rave_lights_runtime.h"
#include <nuttx/config.h>
#include <stdio.h>
#include "thread_init/threads_init.h"
#include "pthread.h"
#include "pubsub-c/pubsub.h"

#include <sdk/config.h>
#include <nuttx/arch.h>

#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/boardctl.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <debug.h>
#include <arch/chip/scu.h>
#include <arch/chip/adc.h>

#define CONFIG_EXAMPLES_ADC_MONITOR_DEVPATH "/dev/lpadc0"

int fd = -1;

int init_adc_reading(void)
{
    int errval, errno;
    fd = open(CONFIG_EXAMPLES_ADC_MONITOR_DEVPATH, O_RDONLY);
    if (fd < 0)
    {
        printf("open %s failed: %d\n", CONFIG_EXAMPLES_ADC_MONITOR_DEVPATH, errno);
        errval = 4;
        return errval;
    }

    /* SCU FIFO overwrite */
    int ret = ioctl(fd, SCUIOC_SETFIFOMODE, 1);
    if (ret < 0)
    {
        errval = errno;
        printf("ioctl(SETFIFOMODE) failed: %d\n", errval);
        close(fd);
    }

    ret = ioctl(fd, ANIOC_CXD56_START, 0);
    if (ret < 0)
    {
        errval = errno;
        printf("ioctl(START) failed: %d\n", errval);
        return errval;
    }

    return 0;
}

int close_adc_reading(void)
{
    int ret = ioctl(fd, ANIOC_CXD56_STOP, 0);
    if (ret < 0)
    {
        int errcode = errno;
        printf("ioctl(STOP) failed: %d\n", errcode);

        return errcode;
    }

    close(fd);

    return 0;
}

#define BUFF_SIZE 16
uint8_t buffer[BUFF_SIZE];
void test_read(void)
{
    int errval, errno;

    int nbytes = read(fd, buffer, BUFF_SIZE);
    if (nbytes < 0)
    {
        errval = errno;
        printf("read failed:%d\n", errval);
        return errval;
    }
    else if (nbytes == 0)
    {
        printf("read data size = 0\n");
    }
    else
    {
        printf("Data: %d", buffer[0]);
    }
}

void rave_light_runtime_main(void *ptr)
{
    init_adc_reading();
    test_read();
    close_adc_reading();
    for (;;)
    {
        usleep(1000);
    }
}