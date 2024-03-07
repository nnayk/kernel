/*
 * Nakul Nayak
 * Kernel Project
 * Description:
*/

#include <stddef.h>
#include "blockdev.h"
#include "process.h"
#include "scheduler.h"
#include "print.h"
#include "utility.h"
#include "memutils.h"
#include "constants.h"
#include "irq.h"

#define DBUG 1
extern ProcQueue *ready_procs;
const uint16_t bus_io_bases[] = {ATA1B1_IO,ATA1B2_IO,ATA2B1_IO,ATA2B2_IO};
const uint16_t bus_ctl_bases[] = {ATA1B1_CTL,ATA1B2_CTL,ATA2B1_CTL,ATA2B2_CTL};
const int data_buff_size = DATA_WD_CT * sizeof(uint16_t); // amount of data to read from ATA data port
ATABD_lst ata_lst;

static ATABD *ATABD_probe(uint16_t io_base,int use_slave)
{
    #define BSY_BIT (1<<7)
    #define STATUS_DRQ (1<<3)
    #define STATUS_ERR 1
    irq_set_mask(14);
    uint64_t sec_ct=0;
    uint8_t status=0;
    ATABD *dev=NULL;
    // specify master channel usage
    outb(io_base+DRIVE,0xA0);
    // set sector count to 0
    outb(io_base+SEC_CT, 0);
    // set LBA ports to 0
    outb(io_base+LBA_LOW, 0);
    outb(io_base+LBA_MID, 0);
    outb(io_base+LBA_HIGH, 0);
    // send 'IDENTIFY' to command port
    outb(io_base+CMD, 0xEC);
    // wait...
    inb(io_base);
    inb(io_base);
    inb(io_base);
    inb(io_base);
    uint8_t cl = inb(io_base+LBA_MID);
    uint8_t ch = inb(io_base+LBA_HIGH);
    /* differentiate ATA, ATAPI, SATA and SATAPI */
	if (cl==0x14 && ch==0xEB) 
    {
            printk("ATADEV_PATAPI\n");
            //return ATADEV_PATAPI;
    }
	if (cl==0x69 && ch==0x96) 
    {
            printk("ATADEV_SATAPI\n");
            //return ATADEV_SATAPI;
    }
	if (cl==0 && ch == 0) 
    {
            printk("ATADEV_PATA\n");
            //return ATADEV_PATA;
    }
	if (cl==0x3c && ch==0xc3) 
    {
            printk("ATADEV_SATA\n");
            //return ATADEV_SATA;
    }

    // send 'IDENTIFY' to command port again
    outb(io_base+CMD, 0xEC);
    // read status register
    status = inb(io_base + STATUS);
    status = inb(io_base + STATUS);
    status = inb(io_base + STATUS);
    status = inb(io_base + STATUS);
    if(DBUG && (status & STATUS_DRQ)) printk("data ready to read\n");
    
    if (status & STATUS_ERR) {
        printk("ATABD_probe: ERR bit set! status = %hx\n",status);
        bail();
    }


    // read data
    uint16_t *buffer = kmalloc(DATA_WD_CT*sizeof(uint16_t));
    uint16_t data;
    for (int i = 0; i < DATA_WD_CT; i++) {
        data = inw(io_base+DATA);
        printk("iteration %d: read %hx\n",i,data);
        buffer[i] = data;
    }
    // verify that LBA48 is supported
    if(!(buffer[83] & (1<<10)))
    {
            printk("ATABD_probe: LBA48 not supported!\n");
            //bail();
    }
    // read and store the number of sectors on the disk
    sec_ct |= (((uint64_t)buffer[100]));
    sec_ct |= (((uint64_t)buffer[101]) << 16);
    sec_ct |= (((uint64_t)buffer[102]) << 32);
    sec_ct |= (((uint64_t)buffer[103]) << 48);
    if(DBUG) printk("%lu sectors total\n",sec_ct);
    dev = (ATABD *)kmalloc(sizeof(ATABD));
    ATABD_init(dev);
    BD_init(&dev->parent);
    dev->parent.sec_ct = sec_ct;
    kfree(buffer);
    irq_clear_mask(14);
    return dev;
}

static void ata_init(void *arg)
{
    ATABD *ata_dev = NULL;   
    ata_lst.size = 0;
    ata_lst.capacity = NUM_ATA_BUSES_PER_CTLR * NUM_CTLRS;
    // probe each bus on the first ATA controller and register ATA devs as appropriate
    for(int i=0;i<NUM_ATA_BUSES_PER_CTLR*NUM_CTLRS;i++)
    {
        if((ata_dev = ATABD_probe(bus_io_bases[i],0))) 
        {
                printk("ata_init: found dev on bus %d\n",i+1);
                ATABD_register(ata_dev);
        }
        else printk("ata_init: didn't find a dev on bus %d\n",i+1);
    }
}

void ATABD_register(ATABD *dev)
{
        if(!dev)
        {
                printk("ATABD_register: null arg!\n");
                bail();
        }

        if(ata_lst.size == ata_lst.capacity)
        {
                printk("ATABD_register: reached list capacity of %d!\n",ata_lst.capacity);
                bail();
        }

        ata_lst.devs[ata_lst.size++] = dev;
}

void setup_ata()
{
    Process *proc = PROC_create_kthread((kproc_t)ata_init,NULL);
    sched_admit(ready_procs,proc);
}


BD *BD_init(BD *self)
{
        self->blk_size = BLK_SIZE;
        self->sec_ct = 0;
        self->read_block = NULL;
        self->next = NULL;
        return self;
}

ATABD *ATABD_init(ATABD *self)
{
        BD_init(&self->parent);
        self->name = NULL;
        self->parent.read_block = &ATABD_read;
        return self;
}

int ATABD_read(BD *dev, uint64_t blk_num, void *dst)
{
     return 1; // change    
}
