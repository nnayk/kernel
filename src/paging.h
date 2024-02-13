/*
 * Nakul Nayak
 * Kernel Project
 * Description:
*/

typedef struct
{
  uint64_t present : 1;
  uint64_t writable : 1;
  uint64_t user_access : 1;
  uint64_t write_through : 1;
  uint64_t cache_disabled : 1;
  uint64_t accessed : 1;
  uint64_t ignore : 3;
  uint64_t avl1 : 3;
  uint64_t address : 40;     // << 12 to get full address
  uint64_t avl2 : 11;  // used for demand paging flags
  uint64_t nx : 1;
} __attribute__((packed)) PTE;

int allocate_pte(void *);
