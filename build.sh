#!/bin/bash

echo Building cos

CURDIR=`pwd`
SOURCEDIR=$CURDIR/kernelsrc
INCLUDE=$SOURCEDIR/include
OBJ=$CURDIR/obj
CFLAGS="-std=gnu99 -ffreestanding -O2 -Wall -Wextra -fdiagnostics-color=auto -Werror=implicit-function-declaration"
OUT=cos.bin
ISOOUT=cos.iso

echo Building the boot asm file
i686-elf-as $SOURCEDIR/boot/boot.s -o obj/boot.o

echo Compiling kernel c files
cd $SOURCEDIR
cd kernel
for i in *.c; do
	echo Compiling $i
	i686-elf-gcc -c $i -o $OBJ/kernel_$i.o $CFLAGS -I$INCLUDE
done
cd asm
echo Assembling kernel/asm asm files
for i in *.asm; do
	echo Assembling $i
	nasm -felf32 $i -o $OBJ/kernel_asm_$i.o
done
cd ..
echo Compiling kernel/display files
cd display
for i in *.c; do
	echo Compiling $i
	i686-elf-gcc -c $i -o $OBJ/kernel_display_$i.o $CFLAGS -I$INCLUDE
done
cd ..
echo Compiling kernel/sys fies
cd sys
for i in *.c; do
	echo Compiling $i
	i686-elf-gcc -c $i -o $OBJ/kernel_sys_$i.o $CFLAGS -I$INCLUDE
done
echo Compiling kernel/sys/memory files
cd memory
for i in *.c; do
	echo Compiling $i
	i686-elf-gcc -c $i -o $OBJ/kernel_sys_memory_$i.o $CFLAGS -I$INCLUDE
done
cd ..
cd ..
echo Compiling kernel/input fies
cd input
for i in *.c; do
	echo Compiling $i
	i686-elf-gcc -c $i -o $OBJ/kernel_input_$i.o $CFLAGS -I$INCLUDE
done
cd ..
cd ..

cd $CURDIR

echo Linking all .o files to $OUT
cd $OBJ
i686-elf-gcc -T linker.ld -o $OUT -ffreestanding -O2 -nostdlib *.o -lgcc
echo cleaning up...
rm ../cos.bin
rm *.o
mv cos.bin ../cos.bin
cd ..

if [ $1 = "qemu" ]; then
	qemu-system-i386 -kernel cos.bin
fi
if [ $1 = "bochs" ]; then
	bochs
fi
if [ $1 = ""]; then
	if grub-file --is-x86-multiboot $OUT; then
		echo multiboot OS in $OUT confirmed, creating iso at $ISOOUT
		mkdir -p $CURDIR/isodir/boot/grub
		cp $OUT $CURDIR/isodir/boot/$OUT
		cp grub/grub.cfg $CURDIR/isodir/boot/grub/grub.cfg
		grub-mkrescue -o $ISOOUT $CURDIR/isodir > /dev/null
	else
		echo $OUT is not multiboot
	fi
fi
if [ $1 = "iso" ]; then
	if grub-file --is-x86-multiboot $OUT; then
		echo multiboot OS in $OUT confirmed, creating iso at $ISOOUT
		mkdir -p $CURDIR/isodir/boot/grub
		cp $OUT $CURDIR/isodir/boot/$OUT
		cp grub/grub.cfg $CURDIR/isodir/boot/grub/grub.cfg
		grub-mkrescue -o $ISOOUT $CURDIR/isodir > /dev/null
	else
		echo $OUT is not multiboot
	fi
fi
