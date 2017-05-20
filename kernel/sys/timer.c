#include <sys/timer.h>
#include <display/term.h>
#include <sys/irqs.h>

//Keeps track of how many ticks our OS has been running for
unsigned int timer_ticks = 0;

/*
 *Handles our timer. In this case it's just a tick counter and prints a message every second
 *that passes. The timer fires at a rate of 18.222Hz, because of some other timing that was
 *happening on the motherboard, blah blah blah...
 *So basically, we just set one second to be about as close as we can get it, 18 ticks
 */
 void timer_handler(regs_t *r)
 {
	//Increment our tick count
	timer_ticks++;

	#if 0
	//Every 18 ticks, we shove a message on the screen
	if (timer_ticks % 100 == 0)
	{
		terminal_writeline("Just about one second has passed");
	}
	#endif
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
void timer_install()
{
	//Sets timer_handler as IRQ0
	irq_install_handler(0, &timer_handler);
	timer_phase(100);
	timer_ticks=0;
}

void timer_reset()
{
	timer_ticks=0;
}

void timer_uninstall()
{
	irq_uninstall_handler(0);
}



void timer_phase(int hz)
{
	int divisor = 1193180 / hz;       /* Calculate our divisor */
	outportb(0x43, 0x36);             /* Set our command byte 0x36 */
	outportb(0x40, divisor & 0xFF);   /* Set low byte of divisor */
	outportb(0x40, divisor >> 8);     /* Set high byte of divisor */
}

