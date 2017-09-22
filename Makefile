SRCFILES := hello.c hello/derp/derp.asm hello/derp.c hello/derp.s hello/hi.c
OBJFILES := $(subst .asm,.o,$(subst .c,.o,$(subst /,_, $(SRCFILES))))

.PHONY: all
all:
	@echo $(OBJFILES)