/*
 * Nakul Nayak
 * Kernel Project
 * Description:
*/
#include <stdint-gcc.h>

#define KHEAP_START_VA (void *)0x10000000000
#define KSTACK_START_VA (void *)0xF0000000000
#define MAX_FRAME_ADDR (void *)0xC0000000 // first 3 GB

typedef struct
{
  uint64_t present:1;
  uint64_t writable:1;
  uint64_t user_access:1;
  uint64_t write_through:1;
  uint64_t cache_disabled:1;
  uint64_t accessed:1;
  uint64_t ignore:3;
  uint64_t alloced:3; // 1 if page has been allocated, 0 if not
  uint64_t addr:40;     
  uint64_t avl2:11;  
  uint64_t nx:1;
} __attribute__((packed)) PTE_t;

typedef struct
{
  uint64_t frame_off:12;
  uint64_t p1_index:9; 
  uint64_t p2_index:9;
  uint64_t p3_index:9;
  uint64_t p4_index:9;
  uint64_t unused:16;
} __attribute__((packed)) VA_t;

typedef enum {
        GET_P1,
        SET_P1,
        GET_PA,
        SET_PA
}PT_op;

// Allocates a new frame and sets the given
// entry to point to this frame
void *alloc_pte(PTE_t *,int);
void *va_to_pa(void *,void *,PT_op);
void *MMU_alloc_page();
void *MMU_alloc_pages(int);
void MMU_free_page(void *);
void MMU_free_pages(void *,int);
void *setup_pt4();
void *get_full_addr(PTE_t *,uint16_t);
void *get_pte_addr(PTE_t *,uint16_t);
int valid_pa(void *);
int valid_va(void *);
void map_kernel_text(void *);
int init_entry(void *frame_start);
void pg_fault_isr(int,int,void *);
