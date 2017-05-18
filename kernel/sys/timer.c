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
 void timer_handler(struct regs *r)
 {
	//Increment our tick count
	timer_ticks++;

	//Every 18 ticks, we shove a message on the screen
	if (timer_ticks % 18 == 0)
	{
		terminal_writeline("Just about one second has passed");
	}
 }
 
 void timer_wait(unsigned int ticks)
 {
	 //Get the next tick count that that many ticks will pass
	 unsigned long eticks;
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
	irq_install_handler(0, *timer_handler);
}
