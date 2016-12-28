#Spaghetti code, phase dos, Linking

#This idea was actually stolen from TheCool1James's LiquiDOS. (Sorry)

#Here we setup our linker
CC=i686-elf-gcc
#What arguments are we gonna be having with our linker today, hm?
LDARGS=-ffreestanding -O2 -nostdlib
#What we're gonna be linking (.o files, obviously)
OBJ = $(wildcard *.o)
# Link 'em all up
cos.bin: $(OBJ)
	$(CC) -T linker.ld -o $@ $(LDARGS) $^ -lgcc
	mv cos.bin ../
