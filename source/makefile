#Welcome to the messy spaghetti code known as Nate's Makefile for cos
#Set all our macros
#Our compiler we will use for our OS (g++)
CC=i686-elf-g++
#Our assembler (GAS)
AS=i686-elf-as
#All the flags we will be using with our C++ compiler.
CFLAGS= -ffreestanding -O3 -Wall -Wextra -Iinclude -fdiagnostics-color=auto

#And now all our searching shit
SOURCES := $(wildcard *.c) $(wildcard **/*.c)
ASM := $(wildcard *.s) $(wildcard **/*.s)

#Directory where we keep all our *.o files.
#It is imperative that we not have any files with the same name (ex. asm/boot.s and kernel/boot.c++)
ODIR=obj

#We get all our prerequisite *.o files from all the source files, but replaced all the ends with the *.o ending
OBJ := $(addprefix $(ODIR)/,$(notdir $(SOURCES:.c=.o))) $(addprefix $(ODIR)/,$(notdir $(ASM:.s=.o))) $(addprefix $(ODIR)/,$(notdir $(BOOT:.asm=.o)))

#Now we compile

#Our multiboot file which starts our OS



#Compile that shit
cos.bin: $(OBJ)
#Here we link all the *.o files together to make the .bin file (see obj/makefile)
	cd obj && make
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


#Clean up this shit. It's pretty nasty.
.PHONY: clean
clean:
	rm -f $(ODIR)/*.o
	rm -f *.bin
	rm -f *.iso
	rm -f asm/*.o
	rm -r isodir
	echo Done!
