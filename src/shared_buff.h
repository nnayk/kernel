/*
 * Nakul Nayak
 * Kernel Project
 * Description:
*/

#include "stdint-gcc.h"

#define BUFF_SIZE 16
typedef struct{
        char buff[BUFF_SIZE];
        char *consumer, *producer;
        uint8_t idle; // 1 = idle
} State;

 void init_state(State *);
