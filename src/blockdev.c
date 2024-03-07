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
const int data_buff_size = DATA_BYTE_CT * sizeof(uint16_t); // amount of data to read from ATA data port
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
    outb(0x1F6,0xA0);
    outb(0x1F2, 0);
    // set LBA ports to 0
    outb(0x1F3, 0);
    outb(0x1F4, 0);
    outb(0x1F5, 0);
    printk("status b4 identify = %d\n",status);
    /*
    if(status & STATUS_ERR) 
    {
            printk("error is set, status = %d\n",status);
            printk("first = %hx\n",inb(0x1F4));
            printk("second = %hx\n",inb(0x1F5));
            status = inb(io_base+STATUS);
    }
    */
    // send 'IDENTIFY' to command port
    outb(0x1F7, 0xEC);
    // read status register
    inb(io_base);
    inb(io_base);
    inb(io_base);
    inb(io_base);
#if 0
    // check if ERR is set
    printk("status after identify = %d\n",status);
    if(status & STATUS_ERR) 
    {
            printk("error is set, status = %d\n",status);
            printk("first = %hx\n",inb(0x1F4));
            printk("second = %hx\n",inb(0x1F5));
            status = inb(io_base+STATUS);
    }
    // if status is zero, drive doesn't exist 
    if (!status) {
        printk("ATABD_probe: no device present\n");
        return NULL;
    }
    // poll status register
    while (status & BSY_BIT){
        status = inb(io_base + STATUS);
    } 
#endif
        uint8_t cl = inb(io_base+LBA_MID);
        uint8_t ch = inb(io_base+LBA_HIGH);
        /* differentiate ATA, ATAPI, SATA and SATAPI */
	if (cl==0x14 && ch==0xEB) 
    {
            printk("ATADEV_PATAPI");
            //return ATADEV_PATAPI;
    }
	if (cl==0x69 && ch==0x96) 
    {
            printk("ATADEV_SATAPI");
            //return ATADEV_SATAPI;
    }
	if (cl==0 && ch == 0) 
    {
            printk("ATADEV_PATA");
            //return ATADEV_PATA;
    }
	if (cl==0x3c && ch==0xc3) 
    {
            printk("ATADEV_SATA");
            //return ATADEV_SATA;
    }
    /*    
    if(inb(io_base+LBA_MID) || inb(io_base+LBA_HIGH)) 
        {
                printk("ATABD_probe: device is not ATA\n");
                return NULL;
        }
    */
#if 0
    // poll status register until DRQ or ERR is set
    while(1)
    {
        if((status & STATUS_DRQ) || (status & STATUS_ERR)) break; 
        status = inb(io_base + STATUS);
    }

    printk("status = %d\n",status);

    if(DBUG && (status & STATUS_DRQ)) printk("data ready to read\n");
    
    if (status & STATUS_ERR) {
        printk("ATABD_probe: ERR bit set!\n");
        bail();
    }

    if(DBUG) printk("found a valid ATA device!\n");
#endif

    // send 'IDENTIFY' to command port
    //outb(io_base + CMD, 0xEC);
    // read status register
    status = inb(io_base + STATUS);
    //status = inb(io_base + STATUS);
    //status = inb(io_base + STATUS);
    //status = inb(io_base + STATUS);
    while(1)
    {
        if((status & STATUS_DRQ) || (status & STATUS_ERR)) break; 
        status = inb(io_base + STATUS);
    }
    if(DBUG && (status & STATUS_DRQ)) printk("data ready to read\n");
    
    if (status & STATUS_ERR) {
        printk("ATABD_probe: ERR bit set! status = %hx\n",status);
        bail();
    }


    // read data
    //uint16_t *buffer = kmalloc(256*sizeof(uint16_t));
    uint16_t data;
    //uint8_t temp;
    //uint8_t temp2;
    for (int i = 0; i < DATA_BYTE_CT; i++) {
        //data = 0;
        data = inw(io_base+DATA);
        //temp = inb(io_base+DATA);
        //temp2 = (inb(io_base+DATA) << 8);
        //printk("temp1=%d\n",temp);
        //data = temp;
        //temp = inb(io_base);
        //printk("temp2=%d\n",temp);
        //data = (temp<<8) | data;
        //printk("iteration %d: read %hx\n",i,temp | temp2);
        printk("iteration %d: read %hx\n",i,data);
        //buffer[i]=temp|temp2;
        //buffer[i] = data;
    }
#if 0
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
#endif
    dev = (ATABD *)kmalloc(sizeof(ATABD));
    ATABD_init(dev);
    BD_init(&dev->parent);
    dev->parent.sec_ct = sec_ct;
    irq_clear_mask(14);
    return dev;
}

static void ata_init(void *arg)
{
    ATABD *ata_dev = NULL;   
    ata_lst.size = 0;
    ata_lst.capacity = NUM_ATA_BUSES_PER_CTLR * NUM_CTLRS;
    // probe each bus on the first ATA controller and register ATA devs as appropriate
    for(int i=0;i<NUM_ATA_BUSES_PER_CTLR*NUM_CTLRS-3;i++)
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
