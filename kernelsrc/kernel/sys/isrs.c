#include <common.h>
#include <sys/isrs.h>
#include <display/term.h>
#include <sys/idt.h>

/* This is a simple string array. It contains the message that
*  corresponds to each and every exception. We get the correct
*  message by accessing like:
*  exception_message[interrupt_number] */
char *exception_messages[] =
{
    "Division By Zero",
    "Debug",
    "Non Maskable Interrupt",
    "Breakpoint",
	"Into Detected Overflow",
	"Out Of Bounds",
	"Invalid Opcode",
	"No Coprocessor",
	"Double Fault",
	"Coprocessor Segment Overrun",
	"Bad TSS Exception",
	"Segment Not Present",
	"Stack Fault",
	"General Protection Fault",
	"Page Fault",
	"Unknown Interrupt",
	"Coprocessor Fault",
	"Alignment Check",	//486+
	"Machine Check",	//Pentium+
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved"
};

void isrs_install()
{
    idt_set_gate(0, (unsigned)isr0, 0x08, 0x8E);
    idt_set_gate(1, (unsigned)isr1, 0x08, 0x8E);
    idt_set_gate(2, (unsigned)isr2, 0x08, 0x8E);
    idt_set_gate(3, (unsigned)isr3, 0x08, 0x8E);
    idt_set_gate(4, (unsigned)isr4, 0x08, 0x8E);
    idt_set_gate(5, (unsigned)isr5, 0x08, 0x8E);
    idt_set_gate(6, (unsigned)isr6, 0x08, 0x8E);
    idt_set_gate(7, (unsigned)isr7, 0x08, 0x8E);
    idt_set_gate(8, (unsigned)isr8, 0x08, 0x8E);
    idt_set_gate(9, (unsigned)isr9, 0x08, 0x8E);
    idt_set_gate(10, (unsigned)isr10, 0x08, 0x8E);
    idt_set_gate(11, (unsigned)isr11, 0x08, 0x8E);
    idt_set_gate(12, (unsigned)isr12, 0x08, 0x8E);
    idt_set_gate(13, (unsigned)isr13, 0x08, 0x8E);
    idt_set_gate(14, (unsigned)isr14, 0x08, 0x8E);
    idt_set_gate(15, (unsigned)isr15, 0x08, 0x8E);
    idt_set_gate(16, (unsigned)isr16, 0x08, 0x8E);
    idt_set_gate(17, (unsigned)isr17, 0x08, 0x8E);
    idt_set_gate(18, (unsigned)isr18, 0x08, 0x8E);
    idt_set_gate(19, (unsigned)isr19, 0x08, 0x8E);
    idt_set_gate(20, (unsigned)isr20, 0x08, 0x8E);
    idt_set_gate(21, (unsigned)isr21, 0x08, 0x8E);
    idt_set_gate(22, (unsigned)isr22, 0x08, 0x8E);
    idt_set_gate(23, (unsigned)isr23, 0x08, 0x8E);
    idt_set_gate(24, (unsigned)isr24, 0x08, 0x8E);
    idt_set_gate(25, (unsigned)isr25, 0x08, 0x8E);
    idt_set_gate(26, (unsigned)isr26, 0x08, 0x8E);
    idt_set_gate(27, (unsigned)isr27, 0x08, 0x8E);
    idt_set_gate(28, (unsigned)isr28, 0x08, 0x8E);
    idt_set_gate(29, (unsigned)isr29, 0x08, 0x8E);
    idt_set_gate(30, (unsigned)isr30, 0x08, 0x8E);
    idt_set_gate(31, (unsigned)isr31, 0x08, 0x8E);
}

/* All of our Exception handling Interrupt Service Routines will
*  point to this function. This will tell us what exception has
*  happened! Right now, we simply halt the system by hitting an
*  endless loop. All ISRs disable interrupts while they are being
*  serviced as a 'locking' mechanism to prevent an IRQ from
*  happening and messing up kernel data structures */
void fault_handler(regs_t *r)
{
 	/* Is this a fault whose number is from 0 to 31? */
    if (r->int_no < 32)
    {
		//Check if the OS is in debug mode
		#if DEBUG == 1
		//Nope
		
		/*Display the current exception and make sure the user is
		 *sufficiently calmed
		 */
			terminal_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
			terminal_clear();
			terminal_writestring(exception_messages[r->int_no]);
			terminal_writestring(" Exception. System Halted!\n");
			terminal_writestring("I know you're scared now... Here's a cat under the moon...\n");
			print_kitty();
			halt();
		
		#else
		//Yep
			terminal_putchar('\n');
			terminal_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
			terminal_writestring(exception_messages[r->int_no]);
			terminal_putchar('[');
			terminal_writehexdword(r->int_no);
			terminal_putchar(']');
			terminal_writestring(" Exception. System Halted!\n");

			regdump(r);
			halt();
		
		#endif
    }
}

void print_kitty()
{
	//Pulled from https://user.xmission.com/~emailbox/ascii_cats.htm
	terminal_writeline("             *     ,MMM8&&&.            *");
	terminal_writeline("                  MMMM88&&&&&    .");
	terminal_writeline("                 MMMM88&&&&&&&");
	terminal_writeline("     *           MMM88&&&&&&&&");
	terminal_writeline("                 MMM88&&&&&&&&");
	terminal_writeline("                 'MMM88&&&&&&'");
	terminal_writeline("                   'MMM8&&&'      *");
	terminal_writeline("          |\\___/|");
	terminal_writeline("          )     (             .              '");
	terminal_writeline("         =\\     /=");
	terminal_writeline("           )===(       *");
	terminal_writeline("          /     \\");
	terminal_writeline("          |     |");
	terminal_writeline("         /       \\");
	terminal_writeline("         \\       /");
	terminal_writeline("  _/\\_/\\_/\\__  _/_/\\_/\\_/\\_/\\_/\\_/\\_/\\_/\\_/\\_");
	terminal_writeline("  |  |  |  |( (  |  |  |  |  |  |  |  |  |  |");
	terminal_writeline("  |  |  |  | ) ) |  |  |  |  |  |  |  |  |  |");
	terminal_writeline("  |  |  |  |(_(  |  |  |  |  |  |  |  |  |  |");
	terminal_writeline("  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |");
	terminal_writeline("  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |");
	
}

void regdump(struct regs *r)
{
	terminal_writestring("gs:");
	terminal_writehexdword(r->gs);
	terminal_writestring(" fs:");
	terminal_writehexdword(r->gs);
	terminal_writestring(" es:");
	terminal_writehexdword(r->es);
	terminal_writestring(" ds:");
	terminal_writehexdword(r->ds);
	terminal_putchar('\n');
}
