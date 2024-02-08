void *memset(void *dst, int c, size_t n);
void *memcpy(void *dest, const void *src, size_t n);
int track_unused();

typedef struct
{
        uint32_t type;
        uint32_t size;
}__attribute__((packed)) tag_hdr;

typedef struct
{
        void *start,*end;
        void *next;
}region;
