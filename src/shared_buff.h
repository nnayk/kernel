/*
 * Nakul Nayak
 * Kernel Project
 * Description:
*/

#define BUFF_SIZE 16
typedef struct{
        char buff[BUFF_SIZE];
        char *consumer, *producer;
} State;

 void init_state(struct State *)
 int consumer_next(struct State *)
 int producer_add_char(char, struct State *)
