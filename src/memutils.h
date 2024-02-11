#define PAGE_SIZE 4096

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
        uint8_t *entry_start;
        
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
        uint32_t *section_name; //unused
        uint32_t type;
        uint64_t flags;
        uint8_t *seg_addr;
        uint64_t seg_off;
        uint64_t seg_size;
        uint32_t table_index;
        uint32_t extra_info;
        uint64_t addr_alignment;
        uint64_t fixed_entry_size;
}__attribute__((packed)) elf_entry_t;

typedef struct
{
        uint8_t *start,*end; // starting and (exclusive) ending addrs of the region
        uint8_t *curr; // current frame offset in the region
        uint8_t *next; // points to next mmap region
}region;

void *memset(void *dst, int c, size_t n);
void *memcpy(void *dest, const void *src, size_t n);
int mem_setup();
int track_unused(memtag_hdr_t *,elftag_hdr_t *);
int setup_unused(memtag_hdr_t,elftag_hdr_t);

