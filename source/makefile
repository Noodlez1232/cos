#Welcome to the messy spaghetti code known as Nate's Makefile for cos
#Set all our macros
#Our compiler we will use for our OS (g++)
CC=i686-elf-g++
#Our assembler (GAS)
GAS=i686-elf-as
#Our secondary assembler (NASM)
NAS=nasm
#All the flags we will be using with our C++ compiler.
CFLAGS= -ffreestanding -O3 -Wall -Wextra -Iinclude -fdiagnostics-color=auto -std=gnu99
#All the flags we will be using with NASM
NFLAGS= -felf

#And now all our searching shit
#C++ files (*.c++)
SOURCES := $(wildcard *.c) $(wildcard **/*.c)
#GAS Files (*.s)
GASM := $(wildcard *.s) $(wildcard **/*.s)
#NASM files (*.asm)
NASM := $(wildcard *.asm) $(wildcard **/*.asm)

#Directory where we keep all our *.o files.
#It is imperative that we not have any files with the same name (ex. asm/boot.s and kernel/boot.c++)
ODIR=obj

#We get all our prerequisite *.o files from all the source files, but replaced all the ends with the *.o ending
#All of our C++ files
OBJC := $(notdir $(SOURCES:.c=.o)) 
#All of our GAS files
OBJA := $(notdir $(ASM:.s=.o)) 
#All of our NASM files
OBJN := $(notdir $(NASM:.asm=.o))
#All obj files
OBJ := $(OBJC) $(OBJA) $(OBJN)

#Now we compile


$(ODIR)/%.o: $(SOURCES)
	echo $(OBJ)
	$(CC) $(SOURCES) -o $(OBJC) $(CFLAGS)
	cp $(OBJC) $(ODIR)/
	$(GAS) $(GASM) -o $(OBJA)
	cp $(OBJA) $(ODIR)/
	$(NAS) $(NFLAGS) $(NASM) -o $(OBJN)
	cp $(OBJN) $(ODIR)/
	ls $(ODIR)



#Compile that shit
cos.bin: $(OBJ)
#Here we link all the *.o files together to make the .bin file (see obj/makefile)
	$(MAKE) $(ODIR)/%.o
	cd obj && $(MAKE)
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
	rm -f kernel/*.o
	rm -r isodir
	echo Done!
