#include <sys/irqs.h>
#include <sys/idt.h>
#include <display/term.h>

isr_t irq_routines[255]; //16 would suffice, but just to be sure...

//This installs a custom IRQ handler for the given IRQ
void irq_install_handler(uint8_t irq, isr_t handler)
{
	#if 0
	terminal_writestring("Installing custom IRQ handler no." );
	terminal_writehexdword(irq);
	terminal_writestring(" Pointed at: ");
	terminal_writehexdword(handler);
	terminal_writestring(" With ISR number: ");
	terminal_putchar('\n');
	#endif
	irq_routines[irq] = handler;
}

//This clears the handler for a given IRQ
void irq_uninstall_handler(uint8_t irq)
{
	irq_routines[irq] = 0;
}


//This is to avoid the IRQs calling the darn exceptions for the CPU
void irq_remap()
{
	outportb(0x20, 0x11);
	outportb(0xA0, 0x11);
	outportb(0x21, 0x20);
	outportb(0xA1, 0x28);
	outportb(0x21, 0x04);
	outportb(0xA1, 0x02);
	outportb(0x21, 0x01);
	outportb(0xA1, 0x01);
	outportb(0x21, 0x0);
	outportb(0xA1, 0x0);
}


//Now we install our IDT entries
void irq_install()
{
	//First we remap the IRQs to be at where we want.
	irq_remap();

	//Then we set all our interupts just like IDT entries
	idt_set_gate(32, (unsigned)irq0, 0x08, 0x8E);
	idt_set_gate(33, (unsigned)irq1, 0x08, 0x8E);
	idt_set_gate(34, (unsigned)irq2, 0x08, 0x8E);
	idt_set_gate(35, (unsigned)irq3, 0x08, 0x8E);
	idt_set_gate(36, (unsigned)irq4, 0x08, 0x8E);
	idt_set_gate(37, (unsigned)irq5, 0x08, 0x8E);
	idt_set_gate(38, (unsigned)irq6, 0x08, 0x8E);
	idt_set_gate(39, (unsigned)irq7, 0x08, 0x8E);
	idt_set_gate(40, (unsigned)irq8, 0x08, 0x8E);
	idt_set_gate(41, (unsigned)irq9, 0x08, 0x8E);
	idt_set_gate(42, (unsigned)irq10, 0x08, 0x8E);
	idt_set_gate(43, (unsigned)irq11, 0x08, 0x8E);
	idt_set_gate(44, (unsigned)irq12, 0x08, 0x8E);
	idt_set_gate(45, (unsigned)irq13, 0x08, 0x8E);
	idt_set_gate(46, (unsigned)irq14, 0x08, 0x8E);
	idt_set_gate(47, (unsigned)irq15, 0x08, 0x8E);
	
	asm volatile("sti"); //Enable all interrupts and allow the IRQ to do its thing
	
}

/*Each IRQ ISR points to this function rather than the fault handler we have in
 *"isrs.c". This is becuase all the IRQ controllers need to have a special 'end of interrupt'
 *or EOI signal(0x20) thrown at them to tell them that you've finished handling the interrupt.
 *This is given to the master controller (at 0x20) in all calses and when an IRQ from 8 to 15
 *is given (meaning it came from the secondary controller, or the slave controller (at 0xA0), it 
 *sends the EOI signal to the slave controller also.
 *If this EOI signal is not given to the IRQ controllers, they will never send an IRQ again.
 */
void irq_handler(regs_t *r)
{
	
	//Check to see if we have a custom IRQ to handle
	if (irq_routines[r->int_no-32] != 0)
	{
		//Get the handler from the array of handlers and call it.
		isr_t handler = irq_routines[r->int_no-32];
		handler(r);
	}

	if (r->int_no == 39)
	{
		return;
	}
	
	//Make sure the slave controller gets an EOI on any IRQs greater than 8 (IDT[40])
	if (r->int_no >= 40)
	{
		slaveEOI();
    }
    masterEOI();

}

void masterEOI()
{
	//Send the EOI to the master controller
	outportb(0x20, 0x20);
}

void slaveEOI()
{
	//Send EOI to the slave controller
	outportb(0xA0, 0x20);
}
