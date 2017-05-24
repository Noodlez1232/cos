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
unsigned int timer_ticks = 0;
uint32_t max_ticks = 4294967295; //Max of 32 bits
bool timer_installed = false;

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
	if (++timer_ticks>=max_ticks)
	{
		clear_interrupts();
		timer_uninstall();
	}
	terminal_writestring("Ticks: ");
	terminal_writehexdword(timer_ticks);
	terminal_putchar('\n');
	terminal_writestring("Max Ticks: ");
	terminal_writehexdword(max_ticks);
	terminal_writeline("");
 }
 
 void timer_wait(unsigned int ticks)
 {
	//Get the next tick count that that many ticks will pass
	timer_ticks=0;
	unsigned int eticks;
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
	timer_installed=true;
}

void timer_reset()
{
	max_ticks = 4294967295;
	timer_ticks=0;
}

void timer_uninstall()
{
	terminal_writeline("Timer uninstalled");
	irq_uninstall_handler(0); //0 is the IRQ for the timer
	timer_installed=false;
	set_interrupts();
}



void timer_phase(uint32_t hz)
{
	int divisor = 1193180 / hz;					//Calculate our divisor
	outportb(0x43, 0x36);						//Send the command byte
	outportb(0x40, (uint8_t) divisor & 0xFF);	//Set low byte of divisor
	outportb(0x40, (uint8_t) divisor >> 8);		//Set high byte of divisor
}

void timer_delay(uint32_t ms)
{
	timer_install(100);
	timer_reset();
	max_ticks=ms;
	//Now we give it a dead loop to do until the timer is actually done
	while (timer_installed);
	terminal_writeline("Done!");
}