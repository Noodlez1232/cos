#!/bin/bash

#Build COS


#build all the source files
for i in asm/*.s; do
	i686-elf-as $i -o $i.o
done

for i in kernel/*.c++; do
	i686-elf-g++ $i -o $i.o -fno-use-linker-plugin  -ffreestanding -O3 -Wall -Wextra -Iinclude -fdiagnostics-color=auto
done

for i in asm/*.asm; do
	nasm -felf $i -o $i.o
done

mv **/*.o /obj
cd obj && make
cd ..
# Make the GRUB iso
	echo Setting up GRUB...
	mkdir -p isodir/boot/grub
#Make the dir to put all our files into
	cp cos.bin isodir/boot/cos.bin
#Put our GRUB config file there
	cp grub/grub.cfg isodir/boot/grub/grub.cfg
#Actually make the file using grub-mkrescue
	echo Making GRUB iso...
	grub-mkrescue -o cos.iso isodir
	echo Cleaning up current mess...
