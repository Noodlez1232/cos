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

#include <display/term.h>

uint32_t timer_ticks;
bool timer_enabled;

void arch_init()
{
    init_gdt();
    init_idt();
    init_isrs();
    init_irqs();
    bochs_magic_breakpoint;
    pit_install(1000, &x86_pit_handler);
    timer_enabled = false;
    timer_ticks = 0;
    terminal_writestring("Arch inited\n");
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
