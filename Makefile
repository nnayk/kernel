image := fat.img
sec_start := 2048s
sec_len := 30720s 
byte_offset := 1048576
loop0 := /dev/loop28
loop1 := /dev/loop29
mnt_pt := /mnt/fatgrub

.PHONY: empty clean lo_status

clean:
	sudo losetup -d $(loop0)	
	sudo losetup -d $(loop1)
	rm $(image)

empty:
	dd if=/dev/zero of=$(image) bs=512 count=32768

partition: empty
	parted $(image) mklabel msdos
	parted $(image) mkpart primary fat32 2048s 30720s
	parted $(image) set 1 boot on

fat_img: partition
	sudo losetup $(loop0) $(image)
	sudo losetup $(loop1) $(image) -o $(byte_offset)
	-sudo mkdir -p $(mnt_pt)
	sudo mkdosfs -F32 -f 2 $(loop1)
	sudo mount $(loop1) $(mnt_pt)
	sudo grub-install --root-directory=$(mnt_pt) --no-floppy --modules="normal part_msdos ext2 multiboot" $(loop0)
	sudo cp -r .img/* $(mnt_pt)
	sudo umount $(mnt_pt)
	sudo losetup -d $(loop0)	
	sudo losetup -d $(loop1)

run: fat.img
	qemu-system-x86_64 -s -drive format=raw,file=fat.img -serial stdio
