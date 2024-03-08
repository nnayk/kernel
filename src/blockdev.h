/*
 * Nakul Nayak
 * Kernel Project
 * Description:
*/

#include <stdint-gcc.h>

#define BLK_SIZE 512
#define NUM_ATA_BUSES_PER_CTLR 2
#define NUM_CTLRS 2
#define DATA_WD_CT 256

#define ATA1B1_IO 0x1F0
#define ATA1B2_IO 0x170
#define ATA1B1_CTL 0x3F6
#define ATA1B2_CTL 0x376
#define ATA2B1_IO 0x01E8
#define ATA2B2_IO 0x0168
#define ATA2B1_CTL 0x03E6
#define ATA2B2_CTL 0x0366

// ATA IO port offsets
// i/o base offsets
#define DATA            0
#define ERR             1
#define FEAT            ERR
#define SEC_CT          2
#define LBA_LOW         3
#define LBA_MID         4
#define LBA_HIGH        5
#define DRIVE           6
#define STATUS          7
#define CMD             STATUS
// ctl base offsets
#define ALT_STATUS      0
#define DEV_CTL         ALT_STATUS
#define DRIVE_ADDR      1

enum BDType { MASS_STORAGE, PARTITION };

struct BD {
   uint64_t blk_ct;
   int (*read_block)(struct BD *, uint64_t, void *);
   uint32_t blk_size;
   enum BDType type;
   const char *name;
   uint8_t fs_type;
   struct BD *next;
};

typedef struct BD BD;

typedef struct{
   BD parent;
   uint16_t ata_base, ata_master; // unused
   uint8_t use_slave, irq; // irq unused
   char *name;
}ATABD;

typedef struct
{
    int size;
    int capacity;
    ATABD *devs[NUM_CTLRS * NUM_ATA_BUSES_PER_CTLR];
}ATABD_dev_lst;
#if 0
struct ATABD_req_t
{
    uint16_t sec_ct; // number of sectors to read
    uint64_t lba48;
    uint16_t *buffer;
    struct ATABD_req_t *next;
}

typedef struct ATABD_req_t ATABD_req_t;

typedef struct
{
    ATABD_req_t *head;        
    ATABD_req_t *tail;        
}ATABD_req_lst;
#endif
BD *BD_init(BD *);
ATABD *ATABD_init(ATABD *);
int ATABD_read_block(BD *, uint64_t, void *);
void ATABD_read_isr(int,int,void *);
void setup_ata();
void ATABD_register(ATABD *);
