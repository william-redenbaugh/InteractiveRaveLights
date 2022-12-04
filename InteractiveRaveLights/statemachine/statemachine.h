#ifndef _STATEMACHINE_H
#define _STATEMACHINE_H
#include "stdio.h"

/**
 * @brief Callback function definition for when an event is triggered
 */
typedef void (*event_fn_t)(int event, void *params, int *next_state);

/**
 * @brief Callback function definition for when we are entering a state in the statemachine
 */
typedef void (*entry_fn_t)(void *params);

/**
 * @brief Callback function definition for when we are exiting a state in the machine
 */
typedef void (*exit_fn_t)(void *params);

#define SM_STATE_NULL -1
#define SM_EVENT_NULL -1
#define SM_STATE_DEBUG_PRINTF

#ifdef SM_STATE_DEBUG_PRINTF
#define SM_PRINTF_DEBUG(N) printf(N)
#else
#define SM_PRITNF_DEBUG(N)
#endif

/**
 * @brief Structure that helps faciliate transistions between states.
 * @param int event that is associated with the transition
 * @param event_fn_t function pointer callback for the transition funciton callback
 * @param int next_state the next state we would want our state machine to transition into
 */
typedef struct sm_transition_t
{
    int event;
    event_fn_t event_fn;
    int next_state;
} sm_transition_t;

/**
 * @brief The structure that defines a state and helps faciliate state transitions
 * @param int state that we are tying our state machine to. to
 * @param sm_transition_t transition map for all the transtions that we allow in this selected state
 * @param entry_fn_t entry function callback
 * @param exit_fn_t exit function callback
 */
typedef struct sm_state_t
{
    int state;
    const sm_transition_t *sm_transitions;
    entry_fn_t entry_fn;
    exit_fn_t exit_fn;
} sm_state_t;

/**
 * @brief Handler that manages an entire statemachine,
 * @param int current_state that our statemachine is in
 * @param sm_state_t* pointer to the statically defined states table that includes all the states in the statemachine
 * @param num_states the amount of states we have in the state machine
 */
typedef struct sm_handle_t
{
    int current_state;
    sm_state_t *states_table;
    int num_states;
} sm_handle_t;

/**
 * @brief Statemachine initialization process: makes sure that our statemachine is valid.
 * @param sm_handle_t handle to the stateamchine
 * @param sm_state_t ointer to the structure including all state events and transitions
 * @param int current_state that we want to start our statemachine on
 */
int init_statemachine(sm_handle_t *sm_handle, const sm_state_t *states, int current_state);

/**
 * @brief Submits an event to a statemachine
 * @param sm_handle_t pointer to the handle for the statemachine we are trying to submit an event to
 * @param int next_event for the event that we are trying to submit
 * @param *params if you want to push through any parameters when you are submiting an event into the statemachine
 */
int submit_event(sm_handle_t *sm_handle, int next_event, void *params);

/**
 * @returns (int) the current state of the stateamchine
 */
int get_hsm_state(sm_handle_t *sm_handle);
#endif