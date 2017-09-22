WARNINGS := -Wall -Wextra
WERRORS := -Werror=implicit-function-declaration

TARGET := i686-elf
CC := $(TARGET)-gcc
AS := $(TARGET)-as
NASM := nasm

CURDIR := $(shell pwd)
PROJDIR := $(shell pwd)/kernelsrc

CFLAGS := -std=gnu99 -ffreestanding -O2 -fdiagnostics-color=auto -function-declaration -lgcc -nostdlib $(WARNINGS) $(WERRORS)
NASMFLAGS := -felf32

PROJDIRS := $(PROJDIR)/boot $(PROJDIR)/kernel $(PROJDIR)/kernel/asm $(PROJDIR)/kernel/display $(PROJDIR)/kernel/input $(PROJDIR)/kernel/sys $(PROJDIR)/kernel/sys/memory
INCDIR := $(PROJDIR)/include

SRCFILES := $(shell find $(PROJDIRS) -type f -name "\*.c")
ASMFILES := $(shell find $(PROJDIRS) -type f -name "\*.asm")
ASFILES := $(shell find $(PROJDIRS) -type f -name "\*.s")

OBJFILES := $(subst .asm,.o,$(subst .c,.o,$(subst /,_, $(SRCFILES) $(ASMFILES) $(ASFILES))))
OBJDIR := $(CURDIR)/obj

all: cos.iso

cos.iso:  cos.bin

cos.bin: $(OFILES)
	

.PHONY: clean
clean:
	
	
backup: clean
	
	
