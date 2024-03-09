/*
 * Nakul Nayak
 * Kernel Project
 * Description:
*/

#define FAT32_TYPE 0xC

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

typedef struct
{
    uint64_t start_clust; // TODO: maybe change data type
      char filename[13];
  uint64_t size;
}Inode;
typedef struct
{
    Inode *root_inode;
    uint32_t *fat_tbl;
    Partition_Entry *partition;
    ATABD *dev;
    Fat_Hdr *fat_hdr;
}SuperBk;

void fat_init();
void parse_mbr();
void display_partition_entry(Partition_Entry);
SuperBk *fat_probe();
void display_fat32(Fat_Hdr *);
void display_bpb(Fat_Bpb *);

