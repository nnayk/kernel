/*
 * Nakul Nayak
 * Kernel Project
 * Description:
*/

#include <stdint-gcc.h>
#include "blockdev.h"
#include "print.h"
#include "memutils.h"
#include "process.h"
#include "utility.h"
#include "fat.h"

#define DBUG 0

Partition_Entry part_entries[4];
extern ATABD_dev_lst ata_lst;
SuperBk *super;
static int inodes_cached = 0;

void fat_init()
{
    if(DBUG) printk("inside fat init\n");
    parse_mbr();
    SuperBk *super = fat_probe();
    super->root_inode = init_inode(super->fat_hdr->root_cluster_number);
    super->root_inode->filename  = char_arr_to_uint16_arr("root",5);
    super->root_inode->children = init_dir();
    if(DBUG) printk("just so I don't get a warning/error: %p\n",super);
    readdir(super->root_inode->start_clust,super->root_inode->children,0);
    //uint16_t x[] = {97,98,99,0};
    //char *x = "/parent1/child1";
    //uint16_t *y = char_arr_to_uint16_arr(x,strlen(x));
    //open(y);
}
void parse_mbr()
{
    BD *dev = (BD *)ata_lst.devs[0];
    uint8_t *buffer = kmalloc(512*sizeof(uint8_t));
    ATABD_read_block(dev,0,(uint16_t *)buffer);
    /*
    for(int i=0;i<256;i++)
    {
        if(DBUG) printk("word %d = %hx\n",i,buffer[i]);        
    }
    */
    //int loop=0;
    //while(!loop);
    //validate signatures
    if(DBUG) printk("buffer[255] = %hx\n",buffer[511]);
    if(((uint16_t *)buffer)[255] != 0xAA55)
    {
        if(DBUG) printk("parse_mbr: invalid mbr signature %hx!\n",buffer[255]);
        bail();
    }
    else if(DBUG) if(DBUG) printk("valid mbr signature\n");

    for(int i=0;i<4;i++)
    {
        memcpy(part_entries+i,buffer+446+i*16,sizeof(Partition_Entry));
        if(DBUG) printk("Partition entry %d contents:\n",i+1);
        display_partition_entry(part_entries[i]);
    }
}

void display_partition_entry(Partition_Entry entry)
{
    if(DBUG) printk("status = %d\n",entry.status);
    if(DBUG) printk("partition type = %x\n",entry.partition_type);
    if(DBUG) printk("first sector lba48 = %d\n",entry.first_sec_lba);
    if(DBUG) printk("number of sectors = %d\n",entry.num_secs);
}

SuperBk *fat_probe()
{
    if(DBUG) printk("inside fat_probe\n");
    super = kmalloc(sizeof(SuperBk));
    super->dev = ata_lst.devs[0];
    super->partition_start = part_entries[0].first_sec_lba;
    super->fat_hdr = kmalloc(sizeof(Fat_Hdr));

    /* read and validate Fat header */
    uint16_t *buffer = kmalloc(sizeof(uint16_t)*256);
    if(DBUG) printk("b4 read\n");
    ATABD_read_block((BD *)super->dev,super->partition_start,buffer);
    if(DBUG) printk("after read\n");
    memcpy(super->fat_hdr,buffer,sizeof(Fat_Hdr));
    if(super->fat_hdr->bpb.bytes_per_sector != BLK_SIZE)
    {
        printk("block size is not 512\n");
        bail();
    }
    super->num_clusters = super->fat_hdr->bpb.tot_sectors * super->fat_hdr->bpb.sectors_per_cluster;
    super->num_secs_per_cluster = super->fat_hdr->bpb.sectors_per_cluster;
    super->fat_tbl = kmalloc(super->fat_hdr->sectors_per_fat*BLK_SIZE);
    uint32_t sector = 0;
    // cache fat table
    for(int i = 0;i<super->fat_hdr->sectors_per_fat;i++)
    {
        sector = super->partition_start+super->fat_hdr->bpb.reserved_sectors+i;
        ATABD_read_block((BD *)super->dev,sector,buffer);
        memcpy(super->fat_tbl+i*128,buffer,BLK_SIZE);
        //printk(" sector %d first 10 bytes:\n",sector);
        /*
        for(int j=0;j<10;j++)
        {
            printk("buffer: %hx\n",buffer[j]);
            printk("fat_tbl: %hx\n",super->fat_tbl[i*128+j] & 0x0FFFFFFF);
        }
        */
    }
    if(super->fat_hdr->signature != 0x28 && super->fat_hdr->signature != 0x29)
    {
        printk("invalid fat32 signature: %hx\n",super->fat_hdr->signature);
        bail();
    }
    display_fat32(super->fat_hdr);
    kfree(buffer);
    return super;   
}

void display_fat32(Fat_Hdr *hdr)
{
    printk("Fat header attributes:\n");
    display_bpb(&hdr->bpb);
    if(hdr->bpb.num_sectors_per_fat == 0)
    {
        printk("real num sectors per fat = %d\n",hdr->sectors_per_fat);
    }

    printk("root dir cluster no. = %d\n",hdr->root_cluster_number);
    printk("name = %s\n",hdr->label);
}

void display_bpb(Fat_Bpb *bpb)
{
    printk("Fat BPB attributes:\n");
    printk("bytes per sector = %d\n",bpb->bytes_per_sector);
    printk("sectors per cluster = %d\n",bpb->sectors_per_cluster);
    printk("reserved sectors = %d\n",bpb->reserved_sectors);
    printk("num fats = %d\n",bpb->num_fats);
    printk("num dirents = %d\n",bpb->num_dirents);
    printk("total sectors = %d\n",bpb->tot_sectors);
    printk("num sectors per fat = %d\n",bpb->num_sectors_per_fat);
}

void readdir(uint32_t cluster,Dir *parent_dir,int num_spaces)
{
    if(!valid_cluster(cluster))
    {
        printk("invalid cluster %d\n",cluster);
    }
    //Inode *curr_inode;
    uint8_t *cbuffer = kmalloc(BLK_SIZE*super->num_secs_per_cluster);
    uint32_t sector;
    ATABD *dev = ata_lst.devs[0];
    Fat_Dir_Ent *dir_ent;
    int e1,e2;
    Fat_LDir_Ent *ldir_ent;
    uint16_t *name = kmalloc(260*sizeof(uint16_t)); // not sure if I can hardcode like this
    int lfn = 0;
    int offset = 0;
    uint32_t dir_off = 0; // dir entry offset in current cluster
    int prev_lfn=0;
    Inode *curr_inode;
    Dir *curr_dir=NULL; // only for use by directory entries of type dir
    //TODO: fix while loop condition
    while(cluster && cluster < 0x0FFFFFF8)
    {
        if(DBUG) printk("cluster=%hx\n",cluster);
        sector = cluster_to_sector(cluster);
        printk("sector = %d\n",sector);
        // read the cluster
        for(int cbuff_off=0;cbuff_off<super->num_secs_per_cluster;cbuff_off++)
        {
            ATABD_read_block((BD *)dev,sector,cbuffer+cbuff_off*BLK_SIZE);
        }
        // process the cluster and print its contents
        dir_off = 0; 
        // get lfn status of last entry of previous cluster
        //if(lfn) prev_lfn = 1;
        //else prev_lfn = 0;
        // process dir entries
        while(1)
        {
            e1 = 0;
            e2 = 0;
            lfn = 0;
            //curr_dir = NULL;
            dir_ent = (Fat_Dir_Ent *)(cbuffer+dir_off); // TODO: check this ptr arithmetic since cbuffer is of type uint32_t *
            // reached end of dir entries
            if(dir_ent->name[0] == 0) 
            {
                if(DBUG) printk("reached end of entries\n");
                break;
            }
            // skip, this entry has been deleted
            else if(dir_ent->name[0] == 0xE5) continue;
            if(!prev_lfn)
            {
                    if(DBUG) printk("num spaces = %d\n",num_spaces);    
                    for(int i=0;i<num_spaces;i++) 
                    {
                        print_char(' ');
                    }
            }
            else if(DBUG) printk("prev_lfn is true\n");
            // lfn check
            if(dir_ent->attr & FAT_ATTR_LFN) 
            {
                    lfn = 1;
                    if(DBUG) printk("LFN!\n");
                    // read until classic entry reached
                    while(1)
                    {
                        ldir_ent = (Fat_LDir_Ent *)(cbuffer+dir_off);
                        // form the name and print it
                        offset = ((ldir_ent->order & 0x3F) -1) * 13;
                        memcpy(name+offset,ldir_ent->first,sizeof(uint16_t)*5);
                        memcpy(name+offset+5,ldir_ent->middle,sizeof(uint16_t)*6);
                        memcpy(name+offset+11,ldir_ent->last,sizeof(uint16_t)*2);
                        dir_off += sizeof(Fat_Dir_Ent);
                        // if reached last lfn entry, read the classic entry and break
                        if((ldir_ent->order & 0x3f) == 1) // & 0x40) TODO: fix this condition
                        {
                            if(DBUG) printk("lfn = ");
                            display_file_name(name);
                            if(dir_off <= 480) 
                            {
                                 dir_ent = (Fat_Dir_Ent *)(cbuffer+dir_off);
                                 break;
                            }
                        }
                        if(dir_off == 512)
                        {
                            prev_lfn = 1;
                            break;
                        }
                    }
            }
            // recursively read the dir
            if(dir_ent->attr & FAT_ATTR_DIRECTORY)
            {
                if(!lfn && !prev_lfn) printk("dir = %s\n",dir_ent->name);
                /*
                for(int w=0;w<strlen(dir_ent->name);w++)
                {
                    printk("index %d = %d\n",w,dir_ent->name[w]);
                }
                */
                                
                e1 = dir_ent->name[0]=='.' && dir_ent->name[1]==' ';
                e2 = dir_ent->name[0]=='.' && dir_ent->name[1]=='.' && dir_ent->name[2]==' ';
                if(DBUG) printk("e1=%d,e2=%d\n",e1,e2);
                if(!e1 && !e2) 
                {
                        if(DBUG) printk("recursing on next cluster = %hx\n",dir_ent->cluster_hi | dir_ent->cluster_lo);
                        if(DBUG) printk("size = %d bytes\n",dir_ent->size);
                        curr_dir = init_dir();
                        readdir(dir_ent->cluster_hi | dir_ent->cluster_lo,curr_dir,num_spaces+4);
                        
                }
            }
            // else if it's a file, just print the name
            else
            {
                curr_dir = NULL;
                // TODO: lfn check
                if(!lfn && !prev_lfn) printk("%s\n",dir_ent->name);
                if(DBUG) printk("size = %d bytes\n",dir_ent->size);

            }
            if(dir_off >= 512) 
            {
                    if(ldir_ent->order == 1) memset(name,0,sizeof(uint16_t)*260);
                    break; // last entry in cluster is an lfn entry
            }
            // at this point we have a classic entry for certain
            prev_lfn = 0;
            // create an inode if the file system hasn't been processed yet
            if(!inodes_cached && (!e1 && !e2)) 
            {
                curr_inode = init_inode(cluster);
                curr_inode->name_len = get_inode_name_len(name);
                curr_inode->filename = kmalloc(sizeof(uint16_t)*curr_inode->name_len);
                // update the inode entries
                memcpy(&curr_inode->filename,name,curr_inode->name_len);
                curr_inode->size = dir_ent->size;
                curr_inode->ctime = dir_ent->ct << 16 | dir_ent->cd;
                curr_inode->atime = dir_ent->ad;
                curr_inode->mtime = dir_ent->mt << 16 | dir_ent->md;
                memset(name,0,sizeof(uint16_t)*260);
                // if the entry is a directory then update its children reference
                if(curr_dir) curr_inode->children = curr_dir;
                parent_dir->add_inode(parent_dir,curr_inode);
            }
            dir_off += sizeof(Fat_Dir_Ent);
            if(dir_off >= 512) 
            {
                break; // reached last entry in cluster (which happens to be a classic entry)
            }
        }
        cluster = super->fat_tbl[cluster];
        // get the next cluster
    }
    kfree(cbuffer);
}

int valid_cluster(uint32_t cluster)
{
    return (cluster >= 2) && (cluster <= (super->num_clusters+2));
}

uint32_t cluster_to_sector(uint32_t cluster)
{
    return  super->partition_start + super->fat_hdr->bpb.reserved_sectors + super->fat_hdr->bpb.num_fats * super->fat_hdr->sectors_per_fat + cluster - 2;             
}

void add_inode(Dir *d,Inode *inode)
{
    int capacity = d->capacity;
    d->inodes[d->count++] = inode;
    if(d->count == capacity)
    {
        d->capacity *= 2;
        d->inodes = krealloc(d->inodes,d->capacity*sizeof(Inode *));
    }
}

Inode *init_inode(uint32_t cluster)
{
        Inode *inode;
        inode = kmalloc(sizeof(Inode));
        inode->size = -1;
        inode->children = NULL;
        inode->start_clust = cluster;
        inode->children = NULL;
        return inode;
}

Dir *init_dir()
{
        Dir *dir;
        dir = kmalloc(sizeof(Dir));
        dir->capacity = 10;
        dir->count = 0;
        dir->inodes = kmalloc(sizeof(Inode *)*10);
        dir->add_inode = add_inode;
        return dir;
}

void display_file_name(const uint16_t *name)
{
    //printk("display_file_name: %p ",name);
    for(int i=0;i<255;i++)
    {
        if(name[i] == 0) break;
        else print_char(name[i]);
    }
    print_char('\n');
}

int get_inode_name_len(const uint16_t *name)
{
    int count = 0;
    for(int i=0;i<255;i++)
    {
        if(name[i] == 0) break;
        count++;
    }
    return count;
}

File *open(uint16_t *fpath)
{
        const uint16_t DELIM = '/'; /* path delimiter */
        uint8_t INITIAL_SIZE = 10; /* initial size of fnames array */
        uint16_t *temp = fpath;
        File *file = NULL;

    if(!fpath)
    {
        printk("open: null arg\n");
        bail();
    }

        Path * path = (Path *)kmalloc(sizeof(Path));
        path->depth = 0;
        path->fnames = (uint16_t **)kmalloc(sizeof(uint16_t *)*INITIAL_SIZE); // TODO: realloc the size when appropriate

        // DELETE:
        for(int i=0;i<260;i++)
        {
            if(fpath[i]==0) break;
            print_char(fpath[i]);
        }

        /* tokenize path string */
        while((temp = strsep(&fpath,&DELIM)))
        {
                if(!DBUG) display_file_name(temp);
                if(temp[0] == 0) continue;
                path->fnames[path->depth] = (uint16_t *)kmalloc(sizeof(uint16_t)*260);
                memcpy(path->fnames[path->depth],temp,260);
                if(!DBUG) display_file_name(path->fnames[path->depth]);
                path->depth += 1;
        }
        
        file = kmalloc(sizeof(File));
        file->inode = fetch_inode(path);
        file->offset = 0;
        return file;
}

/*
 * Description: Return the inode associated with a given file path
 * Params:
 * fpath -- file path
*/
Inode *fetch_inode(Path *fpath)
{
    Inode *curr_inode = super->root_inode; // start from root
    Dir *curr_dir;
    // traverse file system until desired file is reached
    for(int i=0;i<fpath->depth-1;i++)
    {
        // make sure we're dealing with a directory
        if(!curr_inode->children)
        {
            printk("fetch_inode: invalid file path.");
            bail();
        }

        printk("curr inode = ");
        display_file_name(curr_inode->filename);
        
        // search the child inodes 
        curr_dir = curr_inode->children;
        for(int j=0;j<curr_dir->count;j++)
        {
            if(!are_buffers_equal(fpath->fnames[i],curr_dir->inodes[j]->filename,curr_dir->inodes[j]->name_len))
            {
                curr_inode = curr_dir->inodes[j];        
                printk("found next inode ");
                display_file_name(curr_inode->filename);
            }
        }
    }
    
    printk("invalid path\n");
    return NULL;
}
