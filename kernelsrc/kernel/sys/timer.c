//Include all our lovely libraries
#include <sys/timer.h>
#include <display/term.h> //Originally used for some debugging
#include <sys/irqs.h>

/*
 *In order to make sure the unused parameter regs_t *r doesn't raise
 *an unused parameter warning (it's kinda annoying), we tell the compiler
 *to just not do it. Problem solved...
 */
#pragma GCC diagnostic ignored "-Wunused-parameter"

//Keeps track of how many ticks our OS has been running for
volatile uint32_t timer_ticks = 0;
volatile uint32_t max_ticks = 4294967295; //Max of 32 bits

/*
 *Handles our timer. In this case it's just a tick counter and prints a message every second
 *that passes. The timer fires at a rate of 18.222Hz, because of some other timing that was
 *happening on the motherboard, blah blah blah...
 *So basically, we just set one second to be about as close as we can get it, 18 ticks
 */
void timer_handler(regs_t *r)
{
	//Increment our tick count then check if the ticks are at or over
	//our set max
#if 0
	if (++timer_ticks >= max_ticks)
	{
		timer_uninstall();
		terminal_writeline("And then here");

	}
#endif
#if 0
	terminal_writestring("Ticks: ");
	terminal_writehexdword(timer_ticks);
	terminal_putchar('\n');
	terminal_writestring("Max ticks: ");
	terminal_writehexdword(max_ticks);
	terminal_putchar('\n');
#endif
	timer_ticks++;
	send_EOI(0);
}

void timer_wait(uint32_t ticks)
{
	//Get the next tick count that that many ticks will pass
	timer_ticks = 0;
	uint32_t eticks;
	eticks = timer_ticks + ticks;
	//Now wait until that has been fulfilled
	while (timer_ticks < eticks);
}

/*
 *The timer is at IRQ0, so we shall just shove a handler into that, and hope it works
 */
void timer_install(uint32_t frequency)
{
	//Sets timer_handler as IRQ0
	irq_install_handler(0, &timer_handler);
	timer_phase(frequency);
	timer_reset();
	timer_installed = true;
#if DEBUG == 1
	terminal_debug_writeline("Timer installed");
#endif
}

void timer_reset()
{
	max_ticks = 0 - 1;
	timer_ticks = 0;
#if DEBUG == 1
	terminal_debug_writeline("Timer reset");
#endif
}

void timer_uninstall()
{
	irq_uninstall_handler(0); //0 is the IRQ for the timer
	timer_installed = false;
#if DEBUG == 1
	terminal_debug_writeline("Timer uninstalled");
#endif
}

void timer_phase(uint32_t hz)
{
	int divisor = 1193180 / hz; //Calculate our divisor
	outportb(0x43, 0x36); //Send the command byte
	outportb(0x40, divisor & 0xFF); //Set low byte of divisor
	outportb(0x40, divisor >> 8); //Set high byte of divisor
}

void timer_delay(uint32_t ms)
{
	timer_install(100);
	timer_reset();
	timer_wait(ms);
	timer_uninstall();
}

void timer_set_max_ticks(uint32_t max)
{
	max_ticks = max;
}