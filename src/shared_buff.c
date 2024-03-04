/*
 * Nakul Nayak
 * Kernel Project
 * Description:
*/

#include "shared_buff.h"
#include "utility.h"
#include "error.h"

#define DBUG 0

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

int buff_full(State *state)
{
    return (state->producer == (state->consumer - 1)) ||
        (state->consumer == &(state->buff[0]) && state->producer == &(state->buff[BUFF_SIZE-1]));
}

int buff_empty(State *state)
{
    if(!state)
    {
            printk("buff_empty: bad arg\n");
            bail();
    }

    return state->consumer == state->producer;
}

char buff_read(State *state)
{
        char data = *state->consumer++;
        // wrap consumer pointer back to "front" of circular queue
        if (state->consumer >= &state->buff[BUFF_SIZE])
                state->consumer = &state->buff[0];
        return data;
}
int buff_write(State *state, char toAdd)
{
    *state->producer++ = toAdd;
    if (state->producer >= &state->buff[BUFF_SIZE])
    {
        if(DBUG) printk("buffer wrap\n");
        state->producer = &state->buff[0];
    }

    return SUCCESS;
}

void display_buff(State *state)
{
        for(int i=0;i<20;i++)
        {
            printk("%c",mapScanCodeToAscii(state->buff[i]));
        }

        printk("\n");
}
