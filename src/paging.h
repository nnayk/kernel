/*
 * Nakul Nayak
 * Kernel Project
 * Description:
*/
#include <stdint-gcc.h>

typedef struct
{
  uint64_t present:1;
  uint64_t writable:1;
  uint64_t user_access:1;
  uint64_t write_through:1;
  uint64_t cache_disabled:1;
  uint64_t accessed:1;
  uint64_t ignore:3;
  uint64_t avl1:3;
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
    ONE,
    TWO,
    THREE,
    FOUR
} PT_Level;

// Allocates a new frame and sets the given
// entry to point to this frame
int alloc_pte(PTE_t *,int);
void *va_to_pa(void *);
void *MMU_alloc_page();
void *MMU_alloc_pages(int);
void MMU_free_page(void *);
void MMU_free_pages(void *, int);
