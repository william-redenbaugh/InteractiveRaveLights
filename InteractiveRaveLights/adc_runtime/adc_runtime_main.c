#include "adc_runtime_main.h"
#include <nuttx/config.h>
#include <stdio.h>
#include "thread_init/threads_init.h"
#include "pthread.h"
#include "pubsub-c/pubsub.h"
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
#include "shared_constants/shared_constants.h"

#define CONFIG_EXAMPLES_ADC_MONITOR_DEVPATH "/dev/hpadc0"
#define EXPONENTIAL_FILTER_WEIGHT .3

/**
 * @brief Structure containing all relevant ADC structure information for capturing ADC data
*/
typedef struct adc_struct{
    int fd;
    uint16_t *data;
    size_t data_size;
    uint16_t *filtered_data;
    pthread_mutex_t lock;
    pthread_mutex_t signal_mt;
    pthread_cond_t signal_cv;
}adc_struct_t;

static adc_struct_t *adc;

/**
 * @returns new adc structure with allocated filture and data sizes
*/
adc_struct_t *new_adc_struct(size_t data_size){
    adc_struct_t *adc = malloc(sizeof(adc_struct_t));
    adc->data_size = data_size;
    adc->data = malloc(sizeof(uint16_t) * data_size);
    adc->filtered_data = malloc(sizeof(float) * data_size);
    printf("Data size: %d\n", adc->data_size);

    pthread_mutex_init(&adc->lock, NULL);
    pthread_mutex_init(&adc->signal_mt, NULL);
    pthread_cond_init(&adc->signal_cv, NULL);
    return adc;
}

/**
 * @brief just copies data for now, may do more filterng moving forward..
 * @param adc_struct_t pointer to the adc structure containing
*/
void filter_adc_data(adc_struct_t *adc){
    float running_average = (float)adc->data[0];
    float filter_weight = EXPONENTIAL_FILTER_WEIGHT;

    pthread_mutex_lock(&adc->lock);
    memcpy(adc->filtered_data, adc->data, adc->data_size);
    pthread_mutex_unlock(&adc->lock);
}

/**
 * @brief Sets up ADC hardware to start reading dat with correct FIFO buffer sizes
*/
int init_adc_reading(adc_struct_t *adc)
{
    char str[] = CONFIG_EXAMPLES_ADC_MONITOR_DEVPATH;
    int errval, errno;
    adc->fd = open(str, O_RDONLY);
    if (adc->fd < 0)
    {
        printf("open %s failed: %d\n", CONFIG_EXAMPLES_ADC_MONITOR_DEVPATH, errno);
        errval = 4;
        return errval;
    }

    printf("Opened ADC module\n");

    int ret = ioctl(adc->fd, SCUIOC_SETFIFOMODE, 1);
    if (ret < 0)
    {
        errval = errno;
        printf("ioctl(SETFIFOMODE) failed: %d\n", errval);
        close(adc->fd);
    }
    printf("Set FIFMODE complete...\n");

    ret = ioctl(adc->fd, ANIOC_CXD56_FIFOSIZE, adc->data_size * 2);
    if (ret < 0)
    {
        errval = errno;
        printf("ioctl(SETFIFOMODE) failed: %d\n", errval);
        close(adc->fd);
    }
    printf("Set FIFO Buffer size complete\n");

    printf("Handled ID: %d\n", adc->fd);

    ret = ioctl(adc->fd, ANIOC_CXD56_START, 0);
    if (ret < 0)
    {
        errval = errno;
        printf("ioctl(START) failed: %d\n", errval);
    }

    printf("Completed ADC init.\n");
    return 0;
}

/**
 * @brief Shuts deconstructs ADC readings
*/
int close_adc_reading(adc_struct_t *adc)
{
    int ret = ioctl(adc->fd, ANIOC_CXD56_STOP, 0);
    if (ret < 0)
    {
        int errcode = errno;
        printf("ioctl(STOP) failed: %d\n", errcode);

        return errcode;
    }

    close(adc->fd);

    return 0;
}

/**
 * @brief Reads sensor data directly from posix API, copies into adc structure
*/
void read_adc_data(adc_struct_t *adc)
{
    int errval, errno;
    int nbytes = read(adc->fd, adc->data, adc->data_size * 2);
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
        return;
    }
}

/**
 * @brief broadcasts to all blocked threads that new adc data is available to be processed
*/
static inline void signal_adc_newdata(adc_struct_t *adc){
    pthread_cond_broadcast(&adc->signal_cv);
}

void adc_init_func(void *ptr){
    adc = new_adc_struct(ADC_FFT_BUFFER_SIZE);
    init_adc_reading(adc);
}

/**
 * @brief Runtime thread for handling adc data
*/
void adc_runtime_thread(void *ptr)
{
    up_mdelay(100);

    // All the important stuff in this thread
    while(true){
        read_adc_data(adc);
        filter_adc_data(adc);
        signal_adc_newdata(adc);
        usleep(10000);
    }

    close_adc_reading(adc);
}

/**
 * @brief Gets the size of the ADC buffer to copy the data into
*/
size_t adc_filtered_data_size(void){
    pthread_mutex_lock(&adc->lock);
    size_t adc_size = adc->data_size;
    pthread_mutex_unlock(&adc->lock);
    return adc_size;
}

/**
 * @brief Will let us copy over the filtered data from the ADC thread into another buffer osmewhere else
 * @param float* pointer to the buffer we are copying the data into
 * @param size_t size of the buffer that we are copying the data into
*/
void adc_copy_filtered_data(int16_t *input_buffer, size_t input_buffer_size){
    pthread_mutex_lock(&adc->lock);
    memcpy(input_buffer, adc->filtered_data, input_buffer_size);
    pthread_mutex_unlock(&adc->lock);
}

/**
 * @brief Blocking function for external threads to wait until new adc data is available
*/
void block_until_new_adc_data(void){
    pthread_cond_wait(&adc->signal_cv, &adc->signal_mt);
}