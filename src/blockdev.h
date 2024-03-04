/*
 * Nakul Nayak
 * Kernel Project
 * Description:
*/

#define BLK_SIZE 512
#define ATA1_IRQ 14 // for primary bus
#define ATA2_IRQ 15 // for secondary bus

#define ATA1B1_IO 0x1F0
#define ATA1B2_IO 0x170
#define ATA1B1_CTL 0x3F6
#define ATA1B2_CTL 0x376
#define ATA2B1_IO 0x1E8
#define ATA2B2_IO 0x168
#define ATA2B1_CTL 0x3E6
#define ATA2B2_CTL 0x366

enum BDType { MASS_STORAGE, PARTITION };

struct BD {
   uint64_t tot_len;
   int (*read_block)(struct BD *, uint64_t, void *);
   uint32_t blk_size;
   enum BDType type;
   const char *name;
   uint8_t fs_type;
   struct BD *next;
};

typedef struct BD BD;

typedef{
   BD parent;
   uint16_t ata_base, ata_master;
   uint8_t slave, irq;
   char *name;
   // TODO: confirm that this is the read request queue + add later
   //struct ATARequest *req_head, *req_tail;
}ATABD;

BD *BD_init(BD *);
ATABD *ATABD_init(ATABD *);
int ATABD_read(BD *, uint64_t, void *);
