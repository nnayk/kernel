/*
 * Nakul Nayak
 * Kernel Project
 * Description:
*/

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

void parse_mbr();
void display_partition_entry(Partition_Entry);

