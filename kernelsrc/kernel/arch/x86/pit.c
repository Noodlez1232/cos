//Include all our lovely libraries
#include <arch/x86/pit.h>
#include <display/term.h> //Originally used for some debugging
#include <arch/x86/irqs.h>
#include <arch/x86/port.h>

/*
 *In order to make sure the unused parameter regs_t *r doesn't raise
 *an unused parameter warning (it's kinda annoying), we tell the compiler
 *to just not do it. Problem solved...
 */
#pragma GCC diagnostic ignored "-Wunused-parameter"


/*
 *The timer is at IRQ0, so we shall just shove a handler into that, and hope it works
 */
void pit_install(uint32_t frequency, isr_t handler)
{
	//Sets timer_handler as IRQ0
	irq_install_handler(0, handler);
	pit_phase(frequency);
    pit_installed = true;
#if DEBUG == 1
	terminal_debug_writeline("Timer installed");
#endif
}

void pit_uninstall()
{
	irq_uninstall_handler(0); //0 is the IRQ for the timer
	pit_installed = false;
#if DEBUG == 1
	terminal_debug_writeline("Timer uninstalled");
#endif
}

void pit_phase(uint32_t hz)
{
	int divisor = 1193180 / hz; //Calculate our divisor
	outportb(0x43, 0x36); //Send the command byte
	outportb(0x40, divisor & 0xFF); //Set low byte of divisor
	outportb(0x40, divisor >> 8); //Set high byte of divisor
}
