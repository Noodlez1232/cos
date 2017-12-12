#include <common.h>
#include <arch/x86.h>
#include <arch/x86/gdt.h>
#include <arch/x86/interrupts.h>
#include <arch/x86/port.h>
#include <arch/x86/regs.h>
#include <arch/x86/idt.h>

#include <display/term.h>

void arch_init()
{
    init_gdt();
    init_idt();
    terminal_writestring("Arch inited\n");
}
