
#!/bin/bash

###The build script used for building COS

echo Building cos
echo Make sure your current directory is the same directory that all the source files are in

#Set all the variables that we will be using

CURDIR=`pwd`
SOURCEDIR=$CURDIR/kernelsrc
INCLUDE=$SOURCEDIR/include
OBJ=$CURDIR/obj
CFLAGS="-std=gnu99 -ffreestanding -O2 -Wall -Wextra -fdiagnostics-color=auto -Werror=implicit-function-declaration -lgcc -nostdlib"
OUT=cos.bin
ISOOUT=cos.iso
ISODIR=isodir

#setup our clean function to use throught the thingy
function cleanUp
{
	echo Cleaning up...
	#Remove the possible output file that is in obj
	rm $OBJ/$OUT
	#Remove all the object files that are in the obj dir
	rm $OBJ/*.o
	#remove the isodir directory
	rm -r -f $CURDIR/$ISODIR
}

#Removes everything but source files
function prepareForCommit
{
	cleanUp
	#Remove the kernel bin file
	rm $CURDIR/$OUT
	#Remove the iso file
	rm $CURDIR/$ISOOUT
	#Remove all the Bochs stuff
	rm $CURDIR/bochs*.txt
}

#The function that just makes the iso and stuff
function makeIso
{
	echo Making an ISO...
	if grub-file --is-x86-multiboot $CURDIR/$OUT; then
		echo multiboot OS in $CURDIR/$OUT confirmed, creating ISO at $ISOOUT
		mkdir -p $CURDIR/$ISODIR/boot/grub
		cp $OUT $CURDIR/$ISODIR/boot/$OUT
		cp grub/grub.cfg $CURDIR/$ISODIR/boot/grub/grub.cfg
		grub-mkrescue -o $CURDIR/$ISOOUT $CURDIR/$ISODIR --themes=starfield
	else
		echo $CURDIR/$OUT is not multiboot
		echo Unable to create ISO.
	fi
}

#check to see if all they want to do is clean up all the binary files
if [ $1 = "clean" ]; then
	cleanUp
	exit
fi

#Check to see if all they want to do is make the iso
if [ $1 = "makeiso" ]; then
	makeIso
	exit
fi

#prepare for commit
if [ $1 = "prepareforcommit" ]; then
	prepareForCommit
	exit
fi

if [ $1 = "run" ]; then
    if [$2 = "bochs"]; then
        echo running bochs
        bochs
        exit
    fi
    if [$2 = "qemu"]; then
	echo running qemu
	qemu-system-i386 -cdrom $ISOOUT
    fi
fi

if [ $1 = "in-ide" ]; then
    I686_TOOLS=$CURDIR/barebones-toolchain/cross/`uname -m`/bin
    echo Adding $I686_TOOLS to path
    export PATH=$I686_TOOLS:$PATH
fi

#Build the boot file that will be used to boot the machine
echo Building the boot asm file
 i686-elf-as $SOURCEDIR/kernel/arch/x86/boot.s -o obj/boot.o
# nasm -felf $SOURCEDIR/boot/test.asm -o obj/boot.o
# i686-elf-as $SOURCEDIR/boot/test.asm -o obj/boot.o

#Compile all our main kernel files
echo Compiling kernel c files
cd $SOURCEDIR
cd kernel
for i in *.c; do
	echo Compiling $i
	i686-elf-gcc -c $i -o $OBJ/kernel_$i.o $CFLAGS -I$INCLUDE
done
cd ..

#Return to the original directory

cd $CURDIR


#Link all the object files

echo Linking all .o files to $OUT
cd $OBJ
i686-elf-gcc -T linker.ld -o $OUT -ffreestanding -O2 -nostdlib *.o -lgcc
# i686-elf-gcc -T test.ld -o $OUT -ffreestanding -O2 -nostdlib *.o -lgcc

#Move that cos.bin to the dir before
cp cos.bin ../cos.bin

#Clean up the mess that we made
cleanUp
cd ..

#Check all our arguments

#They want to use qemu
if [ $1 = "qemu" ]; then
    makeIso
    echo Running qemu
    qemu-system-i386 -cdrom $ISOOUT
    exit
fi

#They want to use bochs
if [ $1 = "bochs" ]; then
	makeIso
	echo running bochs
	bochs
	exit
fi

#If no argument was passed or "iso" was passed, make an iso
if [ $1 = ""]; then
	makeIso
	exit
fi
if [ $1 = "iso" ]; then
	makeIso
	exit
fi
#They only want the kernel bin file
if [ $1 = "bin" ]; then
	echo only making bin file
	exit
fi