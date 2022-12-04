#include "statemachine.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <debug.h>
#include <nuttx/i2c/i2c_master.h>
#include <pthread.h>
#include "pubsub-c/pubsub.h"
#include "statemachine.h"

typedef enum
{
    SM_TEST_STATE_ONE,
    SM_TEST_STATE_TWO,
    SM_TEST_STATE_THREE,
    SM_TEST_STATE_END
} tests_states_t;

typedef enum
{
    SM_TEST_EVENT_ONE,
    SM_TEST_EVENT_TWO,
    SM_TEST_EVENT_THREE,
    SM_TEST_EVENT_END
} tests_events_t;

void state_one_event_one(void)
{
    printf("State one event one function\n");
}

void state_one_event_two(void)
{
    printf("State one event two function\n");
}

void state_one_event_three(void)
{
    printf("State one event three function\n");
}

void state_two_event_one(void)
{
    printf("State two event one function\n");
}

void state_two_event_two(void)
{
    printf("State two event two function\n");
}

void state_two_event_three(void)
{
    printf("State two event three function\n");
}

void state_three_event_one(void)
{
    printf("State three event one function\n");
}

void state_three_event_two(void)
{
    printf("State three event two function\n");
}

void state_three_event_three(void)
{
    printf("State three event three function\n");
}

void state_one_entry_fn(void)
{
    printf("State one entry function\n");
}

void state_two_entry_fn(void)
{
    printf("State two entry function\n");
}

void state_three_entry_fn(void)
{
    printf("State three entry function\n");
}

void state_one_exit_fn(void)
{
    printf("State one exit function\n");
}

void state_two_exit_fn(void)
{
    printf("State two exit function\n");
}

void state_three_exit_fn(void)
{
    printf("State three exit function\n");
}

static const sm_transition_t state_one_transitions[] = {
    {SM_TEST_EVENT_ONE, state_one_event_one, SM_TEST_STATE_TWO},
    {SM_TEST_EVENT_TWO, state_one_event_two, SM_TEST_STATE_TWO},
    {SM_TEST_EVENT_THREE, state_one_event_three, SM_TEST_STATE_TWO},
    {SM_EVENT_NULL, NULL, SM_STATE_NULL}};

static const sm_transition_t state_two_transitions[] = {
    {SM_TEST_EVENT_ONE, state_two_event_one, SM_TEST_STATE_THREE},
    {SM_TEST_EVENT_TWO, state_two_event_two, SM_TEST_STATE_THREE},
    {SM_TEST_EVENT_THREE, state_two_event_three, SM_TEST_STATE_THREE},
    {SM_EVENT_NULL, NULL, SM_STATE_NULL}};

static const sm_transition_t state_three_transitions[] = {
    {SM_TEST_EVENT_ONE, state_three_event_one, SM_TEST_STATE_ONE},
    {SM_TEST_EVENT_TWO, state_three_event_two, SM_TEST_STATE_ONE},
    {SM_TEST_EVENT_THREE, state_three_event_three, SM_TEST_STATE_ONE},
    {SM_EVENT_NULL, NULL, SM_STATE_NULL}};

static const sm_state_t test_sm_states[] = {
    [SM_TEST_STATE_ONE] = {SM_TEST_STATE_ONE, state_one_transitions, state_one_entry_fn, state_one_exit_fn},
    [SM_TEST_STATE_TWO] = {SM_TEST_STATE_TWO, state_two_transitions, state_two_entry_fn, state_two_exit_fn},
    [SM_TEST_STATE_THREE] = {SM_TEST_STATE_THREE, state_three_transitions, state_three_entry_fn, state_three_exit_fn},
    [SM_TEST_STATE_END] = {SM_STATE_NULL, NULL, NULL, NULL},
};

void test_sm(void)
{
    sm_handle_t handle;
    init_statemachine(&handle, test_sm_states, SM_TEST_STATE_ONE);

    submit_event(&handle, SM_TEST_EVENT_ONE, NULL);
    submit_event(&handle, SM_TEST_EVENT_ONE, NULL);
    submit_event(&handle, SM_TEST_EVENT_ONE, NULL);

    submit_event(&handle, SM_TEST_EVENT_TWO, NULL);
    submit_event(&handle, SM_TEST_EVENT_TWO, NULL);
    submit_event(&handle, SM_TEST_EVENT_TWO, NULL);

    submit_event(&handle, SM_TEST_EVENT_THREE, NULL);
    submit_event(&handle, SM_TEST_EVENT_THREE, NULL);
    submit_event(&handle, SM_TEST_EVENT_THREE, NULL);
}