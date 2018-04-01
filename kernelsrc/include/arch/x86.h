#ifndef X86_ARCH_H
#define X86_ARCH_H

#include <common.h>
#include <arch/x86/regs.h>
#include <arch/x86/multiboot.h>


#define KERNEL_BASE 0xC0000000

typedef struct arch_info_table
{
    uint32_t* multiboot_magic;
    uint32_t* mbt;
    uint32_t* boot_page_table;
} arch_info_table_t;

// Required functions
void arch_init(uint32_t* arch_stuff);
void delay(uint32_t ms);


//used locally
void x86_pit_handler(regs_t *r);
void set_paging();
#endif
