#include <common.h>
#include <arch/x86.h>
#include <arch/x86/gdt.h>
#include <arch/x86/interrupts.h>
#include <arch/x86/port.h>
#include <arch/x86/regs.h>

#include <display/term.h>

void arch_init()
{
    init_gdt();
    terminal_writestring("Arch inited\n");
}
