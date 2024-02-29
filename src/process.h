#ifndef _PROCESS_H
#define _PROCESS_H
/*
 * Nakul Nayak
 * Kernel Project
 * Description:
*/

#include <stdint-gcc.h>

struct Process{
  // minimal register set
  uint64_t rax;
  uint64_t rbx;
  uint64_t rcx;
  uint64_t rdx;
  uint64_t rdi;
  uint64_t rsi;
  uint64_t r8;
  uint64_t r9;
  uint64_t r10;
  uint64_t r11;
  uint64_t r12;
  uint64_t r13;
  uint64_t r14;
  uint64_t r15;
  uint64_t rbp;
  uint64_t rsp;     
  uint64_t rip;    
  uint64_t cs; 
  uint64_t rflags; 
  uint64_t ss;
  uint64_t ds;
  uint64_t es;
  uint64_t fs;
  uint64_t gs;
  uint64_t cr3;
  
  // proc metadata
  uint64_t pid;
  void *stack_start; // not sure if this is needed
  struct Process *allNext;
  struct Process *next; // either next proc in blocked queue or ready queue
};

typedef struct Process Process;

typedef void (*kproc_t)(void*);
void PROC_run(void);
Process *PROC_create_kthread(kproc_t, void*);
void PROC_reschedule(void);
void PROC_init();
void yield();
void kexit();
void yield_isr(int,int,void *);
void kexit_isr(int,int,void *);
#endif
