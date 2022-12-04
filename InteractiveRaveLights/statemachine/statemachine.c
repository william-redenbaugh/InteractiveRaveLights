
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <debug.h>
#include <nuttx/i2c/i2c_master.h>
#include "rgbMatrixI2CDriver/rgbMatrix.h"
#include <pthread.h>
#include "pubsub-c/pubsub.h"
#include "statemachine.h"

int init_statemachine(sm_handle_t *sm_handle, const sm_state_t *states, int current_state)
{
    SM_PRINTF_DEBUG("Initializing statemachine...\n");
    // Make sure we aren't touching any null references.
    if (sm_handle == NULL | states == NULL)
        return EINVAL;

    sm_handle->states_table = states;
    sm_handle->current_state = current_state;

    SM_PRINTF_DEBUG("Setting up state stable and current state\n");

    int n = 0;
    int temp_state = sm_handle->states_table[n].state;
    while (temp_state != SM_STATE_NULL)
    {
        temp_state = sm_handle->states_table[n].state;
        n++;
    }

    if (n == 0)
        return ENODATA;

    SM_PRINTF_DEBUG("Finalizing Statemachine initialization\n");
    sm_handle->num_states = n;

    return 0;
}

int get_hsm_state(sm_handle_t *sm_handle)
{
    return sm_handle->current_state;
}

int submit_event(sm_handle_t *sm_handle, int next_event, void *params)
{
    int current_state = sm_handle->current_state;
    if (current_state == SM_STATE_NULL)
        return;

    //SM_PRINTF_DEBUG("Current state:%d\n", current_state);

    // printf("Current state: %d\n", sm_handle->current_state);

    // Get our list of transitions
    sm_transition_t *selected_transition_table = sm_handle->states_table[current_state].sm_transitions;
    // Based on the current state we are in, we shall select a particular transition table

    // iterate through the transition table until we reach the end or the relevent event callback
    // Or if no event exists we reach the end of the table
    int n = 0;
    while ((selected_transition_table[n].event = !next_event) || selected_transition_table[n].event == SM_EVENT_NULL)
        n++;

    //SM_PRINTF_DEBUG("Event: %d\n", n);

    if (selected_transition_table[n].event == SM_EVENT_NULL)
    {
        SM_PRINTF_DEBUG("No event matched with this current state in the statemachine\n");

        return EIO;
    }
    if (selected_transition_table[n].event == next_event)
    {
        SM_PRINTF_DEBUG("Matching event found for this state in the statemachine, executing...\n ");
        // Once we find the matching event, we find
        int next_state = selected_transition_table[n].next_state;
        // Since we are exiting the previous state, call the previous state function if not null
        if (sm_handle->states_table[current_state].exit_fn != NULL)
            sm_handle->states_table[current_state].exit_fn(params);
        // Call transition
        selected_transition_table[n].event_fn(next_event, params, &next_state);
        // Now we know what the next state will be, so we shall set it in our
        // SM handler
        sm_handle->current_state = next_state;

        // Now that we are entering a new state in our state machine, we can call the next state's entry function if not null.
        if (sm_handle->states_table[sm_handle->current_state].entry_fn != NULL)
            sm_handle->states_table[sm_handle->current_state].entry_fn(params);
        return 0;
    }
    return 0;
}
