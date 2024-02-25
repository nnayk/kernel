#define PAGE_SIZE 4096
#define LOW_REGION_OFF 0
#define HIGH_REGION_OFF 1
#define NUM_POOLS 6

#include <stddef.h>
// generic tag header
typedef struct
{
        uint32_t type;
        uint32_t size;
}__attribute__((packed)) tag_hdr;

typedef struct
{
        uint32_t type;
        uint32_t size;
        uint32_t entry_size;
        uint32_t entry_version;
        uint8_t *entry_start;
}__attribute__((packed)) memtag_hdr_t;

typedef struct
{
        uint32_t type;
        uint32_t size;
        uint32_t entry_count;
        uint32_t entry_size;
        uint32_t str_table_index; // unused
        void *entry_start;
}__attribute__((packed)) elftag_hdr_t;

typedef struct
{
        uint8_t *start_addr;
        uint64_t size;
        uint32_t type;
        uint32_t reserved;
}__attribute__((packed)) memtag_entry_t;

// only seg_addr and seg_size will be used
typedef struct
{
        uint32_t section_name; //unused
        uint32_t type;
        uint64_t flags;
        void *seg_addr;
        uint64_t seg_off;
        uint64_t seg_size;
        uint32_t table_index;
        uint32_t extra_info;
        uint64_t addr_alignment;
        uint64_t fixed_entry_size;
}__attribute__((packed)) elf_entry_t;

struct region
{
        void *start,*end; // starting and (exclusive) ending addrs of the region
        void *curr; // current frame address in the region
        struct region *next; // points to next mmap region
};
typedef struct region region;

struct Block{
    struct Block *next;
};

typedef struct Block Block;

struct KmallocPool{
   int max_size;
   int avail;
   Block *head; // VA corresponding to first block in pool
};

typedef struct KmallocPool KmallocPool;

typedef struct {
   KmallocPool *pool;
   size_t size;
}KmallocExtra;

void *memset(void *dst, int c, size_t n);
void *memcpy(void *dest, const void *src, size_t n);
int mem_setup();
int track_unused(memtag_hdr_t *,elftag_hdr_t *);
int setup_unused(memtag_hdr_t,elftag_hdr_t);
void *pf_alloc();
void pf_simple_test();
int pf_free(void *);
void pf_nonseq_test();
int pf_stress_test();
int are_pages_equal(const void *, const void *);
void *page_align_up(void *);
void *kmalloc(size_t);
void kfree(void *);
int init_pools();
Block *alloc_pool_block(int);
void display_pools();
