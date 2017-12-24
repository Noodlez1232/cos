#ifndef X86_ARCH_H
#define X86_ARCH_H

#include <common.h>
#include <arch/x86/regs.h>
#include <arch/x86/multiboot.h>

// Required functions
void arch_init(uint32_t* arch_stuff, multiboot_info_t* mbt, uint32_t magic);
void delay(uint32_t ms);




//used locally
void x86_pit_handler(regs_t *r);
void set_paging();
#endif
