#ifndef X86_ARCH_H
#define X86_ARCH_H

#include <common.h>
#include <arch/x86/regs.h>

// Required functions
void arch_init();
void delay(uint32_t ms);




//used locally
void x86_pit_handler(regs_t *r);

#endif
