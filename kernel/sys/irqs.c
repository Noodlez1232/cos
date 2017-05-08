#include <sys/irqs.h>
#include <sys/idt.h>

//This installs a custom IRQ handler for the given IRQ
void irq_install_handler(int irq, void (*handler)(struct regs *r))
{
	irq_routines[irq] = handler;
}

//This clears the handler for a given IRQ
void irq_uninstall_handler(int irq)
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
	
	terminal_bootInfo("IRQs Loaded\n", 0);
}

/*Each IRQ ISR points to this function rantehr than the fault handler we have in
 *"isrs.c". This is becuase all the IRQ controllers need to have a special 'end of interrupt'
 *or EOI signal(0x20) thrown at them to tell them that you've finished handling the interrupt.
 *This is given to the master controller (at 0x20) in all calses and when an IRQ from 8 to 15
 *is given (meaning it came from the secondary controller, or the slave controller (at 0xA0), it 
 *sends the EOI signal to the slave controller also.
 *If this EOI signal is not given to the IRQ controllers, they will never send an IRQ again.
 */
void irq_handler(struct regs *r)
{
	//Just a blank function handler to use to call things.
	void (*handler)(struct regs *r);

	//See if we have a custom handler to run the IRQ and if so, run it, and if not, don't
	handler = irq_routines[r->int_no - 32];
	if (handler)
	{
		handler(r);
	}

	//Make sure the slave controller gets an EOI on any IRQs greater than 8 (IDT[40])
	if (r->int_no >= 40)
	{
		outportb(0xA0, 0x20);
    }

	//Send the EOI to the master controller
	outportb(0x20, 0x20);
}
