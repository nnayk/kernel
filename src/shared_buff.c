/*
 * Nakul Nayak
 * Kernel Project
 * Description:
*/

#include "shared_buff.h"

void init_state(State *state)
{
        state->consumer = &state->buff[0];
        state->producer = &state->buff[0];
        state->idle = 1;
}

void show_state(State *state)
{
        printk("consumer = %d, producer = %d, idle = %d\n",
                        (int)(state->consumer-state->buff),(int)(state->producer-state->buff),state->idle);
}
