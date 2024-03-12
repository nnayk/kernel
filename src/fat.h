/*
 * Nakul Nayak
 * Kernel Project
 * Description:
*/

#define FAT32_TYPE 0xC

#define FAT_ATTR_READ_ONLY 0x01
#define FAT_ATTR_HIDDEN 0x02
#define FAT_ATTR_SYSTEM 0x04
#define FAT_ATTR_VOLUME_ID 0x08
#define FAT_ATTR_DIRECTORY 0x10
#define FAT_ATTR_ARCHIVE 0x20
#define FAT_ATTR_LFN (FAT_ATTR_READ_ONLY | FAT_ATTR_HIDDEN | FAT_ATTR_SYSTEM | FAT_ATTR_VOLUME_ID)

typedef struct
{
    uint8_t status;      // 0x80 = active/bootable, 0x00 = inactive, else invalid
  uint8_t first_sec_head;  // CHS, ignore
  uint8_t first_sec_high;  // CHS, ignore
  uint8_t first_sec_low;   // CHS, ignore
  uint8_t partition_type;
  uint8_t last_sec_head;      // CHS, ignore
  uint8_t last_sec_high;  // CHS, ignore
  uint8_t last_sec_low;  // CHS, ignore
  uint32_t first_sec_lba;
  uint32_t num_secs;            
}Partition_Entry;

typedef struct{
uint8_t jmp[3];
char oem_id[8];
uint16_t bytes_per_sector;
uint8_t sectors_per_cluster;
uint16_t reserved_sectors;
uint8_t num_fats;
uint16_t num_dirents;
uint16_t tot_sectors;
uint8_t mdt;
uint16_t num_sectors_per_fat;
uint16_t sectors_per_track;
uint16_t num_heads;
uint32_t num_hidden_sectors;
uint32_t large_sector_count;
} __attribute__((packed)) Fat_Bpb;

typedef struct{
Fat_Bpb bpb;
uint32_t sectors_per_fat;
uint16_t flags;
uint8_t major_vers;
uint8_t minor_vers;
uint32_t root_cluster_number;
uint16_t fsinfo_sector;
uint16_t backup_boot_sector;
uint8_t zero[12];
uint8_t drive_num;
uint8_t nt_flags;
uint8_t signature;
uint32_t serial_num;
char label[11];
char sys_id[8];
uint8_t boot_code[420];
uint8_t boot_sig[2];
} __attribute__((packed)) Fat_Hdr;

typedef struct Inode Inode;

struct Dir
{
    int capacity; // max number of children that can be stored
    int count; //number of children
    Inode **inodes; // list of child inodes
    void (*add_inode)(struct Dir *,Inode *);
};

typedef struct Dir Dir;

struct Inode
{
    uint32_t start_clust; 
    char *filename;
    int name_len;
    uint64_t size; // in bytes
    uint32_t ctime;
    uint32_t atime;
    uint32_t mtime;
    Dir *children;
};

typedef struct
{
    Inode *root_inode;
    uint32_t *fat_tbl;
    uint32_t partition_start; // start sector for partition
    ATABD *dev;
    Fat_Hdr *fat_hdr;
    int num_clusters;
    int num_secs_per_cluster;
}SuperBk;

typedef struct
{
    Inode *inode;
    uint64_t offset;
}File;

typedef struct{
char name[11];
uint8_t attr;
uint8_t nt;
uint8_t ct_tenths;
uint16_t ct;
uint16_t cd;
uint16_t ad;
uint16_t cluster_hi;
uint16_t mt;
uint16_t md;
uint16_t cluster_lo;
uint32_t size;
} __attribute__((packed)) Fat_Dir_Ent;

typedef struct{
uint8_t order;
uint16_t first[5];
uint8_t attr;
uint8_t type;
uint8_t checksum;
uint16_t middle[6];
uint16_t zero;
uint16_t last[2];
} __attribute__((packed)) Fat_LDir_Ent;

void fat_init();
void parse_mbr();
void display_partition_entry(Partition_Entry);
SuperBk *fat_probe();
void display_fat32(Fat_Hdr *);
void display_bpb(Fat_Bpb *);
void readdir(uint32_t,Dir *,int);
//void get_cluster_chain(uint32_t);
int  valid_cluster();
uint32_t cluster_to_sector(uint32_t);
void add_inode(Dir *,Inode *);
Inode *init_inode(uint32_t);
Dir *init_dir();
void display_file_name(const uint16_t *);
Inode *fetch_inode(Path *);
File *open(uint16_t *);
int get_inode_name_len(const uint16_t *);
