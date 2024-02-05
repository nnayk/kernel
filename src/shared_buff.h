/*
 * Nakul Nayak
 * Kernel Project
 * Description:
*/

#define BUFF_SIZE 16
struct State {
        char buff[BUFF_SIZE];
        char *consumer, *producer;
};
