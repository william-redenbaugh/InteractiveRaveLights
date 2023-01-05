#include "coprocessor_ipc.h"
#include <pthread.h>
#include "../stripCoProcessor/include/coprocessor_ipc_shared.h"
#include "stdio.h"
#include "fcntl.h"

static pthread_mutex_t input_buffer_mutex;
static pthread_mutex_t output_buffer_mutex;
static pthread_cond_t trigger_update;
static uint8_t input_buffer[INPUT_BUFFER_SIZE];
static uint8_t output_buffer[INPUT_BUFFER_SIZE];
static int serial_fd = -1;

void coprocessor_ipc_init(void *params)
{
    pthread_mutex_init(&input_buffer_mutex, NULL);
    pthread_mutex_init(&output_buffer_mutex, NULL);
    pthread_cond_init(&trigger_update, NULL);
    serial_fd = open("/dev/ttyS2", O_RDWR);

    if (serial_fd < 0)
    {
        printf("Error setting up UART IPC to Coprocessor\n");
    }
}

void coprocessor_ipc_thread(void *params)
{

    for (;;)
    {
        pthread_mutex_lock(&output_buffer_mutex);
        pthread_cond_wait(&trigger_update, &output_buffer_mutex);
        // Write to serial buffer
        int ret = write(serial_fd, output_buffer, INPUT_BUFFER_SIZE);
        printf("updated strip\n");
        pthread_mutex_unlock(&output_buffer_mutex);
        usleep(10000);
    }
}

void coprocessor_ipc_led_strip_update_all_threads(void)
{
    pthread_mutex_lock(&input_buffer_mutex);
    pthread_mutex_lock(&output_buffer_mutex);
    memcpy(output_buffer, input_buffer, INPUT_BUFFER_SIZE);
    pthread_mutex_unlock(&output_buffer_mutex);
    pthread_cond_broadcast(&trigger_update);
    pthread_mutex_unlock(&input_buffer_mutex);
}

void coprocesor_ipc_led_strip_set(led_strip_t strip, int n, uint8_t r, uint8_t g, uint8_t b)
{
    int pos = ((int)strip * NUM_COLS_PER_PIXEL * NUM_PIXELS_PER_STRIP) + n * 3;
    pthread_mutex_lock(&input_buffer_mutex);
    input_buffer[pos] = r;
    input_buffer[pos + 1] = g;
    input_buffer[pos + 2] = b;
    pthread_mutex_unlock(&input_buffer_mutex);
}

