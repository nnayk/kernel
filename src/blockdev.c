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

#define DBUG 1
extern ProcQueue *ready_procs;
const uint16_t bus_io_bases[] = {ATA1B1_IO,ATA1B2_IO,ATA2B1_IO,ATA2B2_IO};
const uint16_t bus_ctl_bases[] = {ATA1B1_CTL,ATA1B2_CTL,ATA2B1_CTL,ATA2B2_CTL};
const int data_buff_size = DATA_BYTE_CT * sizeof(uint16_t); // amount of data to read from ATA data port
ATABD_lst ata_lst;


static ATABD *ATABD_probe(uint16_t io_base,int use_slave)
{
    #define BSY_BIT (1<<7)
    #define STATUS_DRQ (1<<3)
    #define STATUS_ERR 1
    uint64_t sec_ct=0;
    uint8_t status=0;
    ATABD *dev=NULL;
    // specify master channel usage
    outb(io_base+DRIVE,0xA0);
    // set LBA ports to 0
    outb(io_base + LBA_LOW, 0);
    outb(io_base + LBA_MID, 0);
    outb(io_base + LBA_HIGH, 0);
    // send 'IDENTIFY' to command port
    outb(io_base + CMD, 0xEC);
    // read status register
    status = inb(io_base + STATUS);
    // if status is zero, drive doesn't exist 
    if (!status) {
        printk("ATABD_probe: no device present\n");
        return NULL;
    }
    // poll status register
    while (status & BSY_BIT){
        status = inb(io_base + STATUS);
        if(inb(io_base+LBA_MID) || inb(io_base+LBA_HIGH)) 
        {
                status = -1;
                break;
        }
    } 
    if (!status) {
        printk("ATABD_probe: no device present\n");
        return NULL;
    }

    // check that device type is ATA
    if (inb(io_base + LBA_MID) || inb(io_base + LBA_HIGH)) {
        printk("ATABD_probe: Device is not type ATA\n");
        return NULL;
    }

    // poll status register until DRQ or ERR is set
    while(1)
    {
        status = inb(io_base + STATUS);
        if((status & STATUS_DRQ) || (status & STATUS_ERR)) break; 
    }

    printk("status = %d\n",status);

    if(DBUG && (status & STATUS_DRQ)) printk("data ready to read\n");
    
    if (status & STATUS_ERR) {
        printk("ATABD_probe: ERR bit set!\n");
        bail();
    }

    if(DBUG) printk("found a valid ATA device!\n");
    dev = (ATABD *)kmalloc(sizeof(ATABD));
    BD_init(&dev->parent);
    ATABD_init(dev);

    // read data
    uint16_t *buffer = kmalloc(256*16);
    uint16_t data;
    uint8_t temp;
    for (int i = 0; i < DATA_BYTE_CT; i++) {
        //data = inw(io_base+DATA);
        temp = inb(io_base);
        printk("temp1=%d\n",temp);
        data = temp;
        temp = inb(io_base);
        printk("temp2=%d\n",temp);
        data = (temp<<8) | data;
        printk("iteration %d: read %d\n",i,data);
        buffer[i]=data;
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
    dev->parent.sec_ct = sec_ct;
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
