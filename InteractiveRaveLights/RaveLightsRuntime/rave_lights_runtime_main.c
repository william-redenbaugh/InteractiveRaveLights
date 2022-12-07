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

#define CONFIG_EXAMPLES_ADC_MONITOR_DEVPATH "/dev/hpadc0"

int fd = -1;

int init_adc_reading(void)
{
    char str[] = CONFIG_EXAMPLES_ADC_MONITOR_DEVPATH;
    int errval, errno;
    fd = open(str, O_RDONLY);
    if (fd < 0)
    {
        printf("open %s failed: %d\n", CONFIG_EXAMPLES_ADC_MONITOR_DEVPATH, errno);
        errval = 4;
        return errval;
    }

    printf("Opened ADC module\n");

    int ret = ioctl(fd, SCUIOC_SETFIFOMODE, 1);
    if (ret < 0)
    {
        errval = errno;
        printf("ioctl(SETFIFOMODE) failed: %d\n", errval);
        close(fd);
    }
    printf("Set FIFMODE complete...\n");

    printf("Handled ID: %d\n", fd);

    ret = ioctl(fd, ANIOC_CXD56_START, 0);
    if (ret < 0)
    {
        errval = errno;
        printf("ioctl(START) failed: %d\n", errval);
    }

   printf("Completed ADC init.\n");
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

void test_read(void)
{
    int errval, errno;
    uint8_t data[2];
    int nbytes = read(fd, data, 2);
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
        printf("Data: %d\n", data[1]);
    }
}

void rave_light_runtime_main(void *ptr)
{
    init_adc_reading();
    up_mdelay(1000);
    while(true){
        //test_read();
        usleep(10000);
    }
    close_adc_reading();
    printf("ADC init complete.\n");

    for(;;){
        usleep(10000);
    }

}