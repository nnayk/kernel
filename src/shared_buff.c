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
}
