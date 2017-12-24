#include <common.h>
#include <arch/x86.h>
#include <arch/x86/gdt.h>
#include <arch/x86/interrupts.h>
#include <arch/x86/port.h>
#include <arch/x86/regs.h>
#include <arch/x86/idt.h>
#include <arch/x86/isrs.h>
#include <arch/x86/irqs.h>
#include <arch/x86/pit.h>
#include <arch/x86/paging.h>
#include <arch/x86/multiboot.h>

#include <display/term.h>

uint32_t timer_ticks;
bool timer_enabled;

uint32_t pt1[4096];
uint32_t pt2[4096];

extern uint8_t _boot_pagedir asm("_boot_pagedir");
extern uint32_t* _boot_pagtab1 asm ("_boot_pagetabl1");
extern uint32_t kernel_start;

void arch_init(uint32_t* arch_stuff, multiboot_info_t* mbt, uint32_t magic)
{
    init_gdt();
    init_idt();
    init_isrs();
    init_irqs();
    set_paging();
//     pit_install(1000, &x86_pit_handler);
//     timer_enabled = false;
//     timer_ticks = 0;
    terminal_writestring("Arch inited\n");
}

void set_paging()
{
    //First thing we do is get the address of the boot_pagedir
    uint32_t boot_pagedir_phys = 0;
    unsigned long ptindex = (unsigned long)_boot_pagtab1 >> 12 & 0x03FF;
    boot_pagedir_phys = _boot_pagtab1[ptindex];
    terminal_debug_writehexdword(boot_pagedir_phys);
}

void delay(uint32_t ms)
{
    timer_enabled = true;
    while (timer_ticks < ms);
    timer_enabled = false;
}



void x86_pit_handler(regs_t* r)
{
    if (timer_enabled)
    {
        timer_ticks++;
//         terminal_writehexdword(timer_ticks);
    }
    send_EOI(0);
}
