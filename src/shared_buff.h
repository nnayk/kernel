/*
 * Nakul Nayak
 * Kernel Project
 * Description:
*/

#ifndef SHARED_BUFF_H
#define SHARED_BUFF_H
#include "stdint-gcc.h"
# include "print.h"

#define BUFF_SIZE 16
typedef struct{
        char buff[BUFF_SIZE];
        char *consumer, *producer;
        uint8_t idle; // 1 = idle
} State;

 void init_state(State *);
void show_state(State *);
#endif
