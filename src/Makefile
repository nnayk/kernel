image := fat.img
sec_start := 2048s
sec_len := 30720s 
byte_offset := 1048576
loop0 := /dev/loop37
loop1 := /dev/loop38
mnt_pt := /mnt/fatgrub
execs := boot multiboot_header
tests = kmalloc_tests.o page_frame_tests.o mt_tests.o kbd_tests.o paging_tests.o
objs := multiboot_header.o boot.o long_mode_init.o kernel.o vga.o utility.o print.o ps2.o idt_init.o irq.o isr.o shared_buff.o serial.o memutils.o paging.o process.o scheduler.o snakes.o blockdev.o $(tests)

my-gcc := $(HOME)/opt/cross/bin/$(TARGET)-gcc
my-gcc_flags := -c -g -Wall -Werror -mno-red-zone

.PHONY: empty clean lo_status

run: $(image)
	qemu-system-x86_64 -s -drive format=raw,file=fat.img -no-reboot -no-shutdown -serial stdio #1>qemu-data-dump -d int #2>qemu-int-dump
clean:
	rm *.o
	rm kernel.bin
	rm $(image)
cleano:
	rm $(objs)

multiboot_header.o: multiboot_header.asm boot.asm
	nasm -f elf64 -F dwarf -g multiboot_header.asm

boot.o: boot.asm boot.asm
	nasm -f elf64 -F dwarf -g boot.asm

long_mode_init.o: long_mode_init.asm long_mode_init.asm
	nasm -f elf64 -F dwarf -g long_mode_init.asm

idt_init.o: idt_init.asm idt_init.asm
	nasm -f elf64 -F dwarf -g idt_init.asm

isr.o: isr.asm
	nasm -f elf64 -F dwarf -g isr.asm

.c.o:
	$(my-gcc) $(my-gcc_flags) $< -o $@
# binary file with dummy "OKAY" display for now
kernel.bin: linker.ld $(objs) 
	ld -n -o $(@) -T $^

# package the source for the kernel image (includes grub bootloader)
kernel_package: kernel.bin grub.cfg
	mkdir -p .img/boot/grub
	cp grub.cfg .img/boot/grub
	cp kernel.bin .img/boot
        	
empty:
	dd if=/dev/zero of=$(image) bs=512 count=32768

partition: empty
	parted $(image) mklabel msdos
	parted $(image) mkpart primary fat32 2048s 30720s
	parted $(image) set 1 boot on

$(image): partition kernel_package
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

