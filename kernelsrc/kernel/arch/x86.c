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
#include <arch/x86/mm/paging.h>
#include <arch/x86/multiboot.h>

#include <display/term.h>

uint32_t timer_ticks;
bool timer_enabled;

uint32_t pt1[1024] __attribute__ ((aligned (4096)));
uint32_t pd1[1024] __attribute__ ((aligned (4096)));

extern uint32_t _kernel_start;
extern uint32_t _kernel_base;

void arch_init(uint32_t* arch_stuff)
{
    init_gdt();
    init_idt();
    init_isrs();
    init_irqs();
    set_paging(arch_stuff[2]);
//     pit_install(1000, &x86_pit_handler);
//     timer_enabled = false;
//     timer_ticks = 0;
    terminal_writestring("Arch inited\n");
}

void set_paging(uint32_t* boot_dir)
{
	asm volatile ("xchg %%bx, %%bx");
	terminal_debug_writeline("PT");
	terminal_debug_writehexdword(pt1);
	terminal_debug_putchar('\n');
	terminal_debug_writeline("PD");
	terminal_debug_writehexdword(pd1);
    //Set all of them to present and read write
    uint16_t flags = P | RW;
    asm volatile("movl %0, %%eax" : : "r"(pt1) : );
    for (uint32_t i = 0; i<1024; i++)
    {
        //Map the first physical 4MB to the table
        pt1[i] = set_pte(i * 4096, flags);
    }
    asm volatile("xchg %bx, %bx");
    //Map the VGA driver to the final mapping
    pt1[1023] = set_pte(0xB000, flags);
    //And we set up our page table
    pd1[(_kernel_base >> 21)] = set_pde(pt1 - _kernel_base, flags);
    pd1[0] = set_pde(pt1 - _kernel_base, flags);
    load_pd(pd1 - _kernel_base);
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
