/*
 * Nakul Nayak
 * Kernel Project
 * Description:
*/

#include "shared_buff.h"

static void init_state(struct State *state)
{
        state->consumer = &state->buff[0];
        state->producer = &state->buff[0];
}

static void consumer_next(struct State *state)
{
        // empty buffer
        if (state->consumer == state->producer)  return;
        // this is a wrapper around outb that writes to serial output
        consume_byte(*state->consumer++);
        // wrap consumer pointer back to "front" of circular queue
        if (state->consumer >= &state->buff[BUFF_SIZE]) 
                state->consumer = &state->buff[0];
}

static int producer_add_char(char toAdd, struct State *state)
{
        // full buffer, throw away the input char and return.
        if ((state->producer == state->consumer - 1) ||
        (state->consumer == &buff[0] && state->producer == &buff[BUFF_SIZE-1]) )
            return;
        *state->producer++ = toAdd;
        if (state->producer >= &state->buff[BUFF_SIZE])
            state->producer = &state->buff[0];
}
