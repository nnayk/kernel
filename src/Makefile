image := fat.img
sec_start := 2048s
sec_len := 30720s 
byte_offset := 1048576
loop0 := /dev/loop26
loop1 := /dev/loop27
mnt_pt := /mnt/fatgrub
execs := boot multiboot_header
objs := multiboot_header.o boot.o

.PHONY: empty clean lo_status

clean_all:
	rm $(image)
	rm kernel.bin
	rm $(execs)
	rm *.o
clean:
	rm $(execs)
	rm $(objs)

multiboot_header.o: multiboot_header.asm boot.asm
	nasm -f elf64 multiboot_header.asm

boot.o: boot.asm boot.asm
	nasm -f elf64 boot.asm

kernel.bin: linker.ld $(objs) 
	ld -n -o $(@) -T $^

kernel: kernel.bin grub.cfg
	mkdir -p .img/boot/grub
	cp grub.cfg .img/boot/grub
	cp kernel.bin .img/boot
        	
empty:
	dd if=/dev/zero of=$(image) bs=512 count=32768

partition: empty
	parted $(image) mklabel msdos
	parted $(image) mkpart primary fat32 2048s 30720s
	parted $(image) set 1 boot on

$(image): partition kernel
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

run: $(image)
	qemu-system-x86_64 -s -drive format=raw,file=fat.img -serial stdio
