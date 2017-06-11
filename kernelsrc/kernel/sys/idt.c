/*
 *The IDT handler.
 *I'll be honest, most if not all of this code was from Bran's kernel development tutorial
 *http://www.osdever.net/bkerndev/Docs/idt.htm
 */

#include <sys/idt.h>
#include <display/term.h>

//Declare our IDT entries of 256

//Even though only 32 will be used for now, the rest act as a sort of trap so that there's no
//sort of funny stuff happening if somebody calls an undefined IRQ
struct idt_entry idt[256];
struct idt_ptr idtp;


/* Use this function to set an entry in the IDT. Alot simpler
*  than twiddling with the GDT ;) */
void idt_set_gate(unsigned char num, unsigned long base, unsigned short sel, unsigned char flags)
{
    idt[num].base_low = base & 0xFFFF;
    idt[num].base_high = (base >> 16) & 0xFFFF;

    idt[num].sel     = sel;
    idt[num].always0 = 0;

    idt[num].flags   = flags /* | 0x60 */;
}

/* Installs the IDT */
void init_idt()
{
    /* Sets the special IDT pointer up, just like in 'gdt.c' */
    idtp.limit = (sizeof (struct idt_entry) * 256) - 1;
    idtp.base = (uint32_t*)&idt;

    /* Clear out the entire IDT, initializing it to zeros */
    memset(&idt, 0, sizeof(struct idt_entry) * 256);

    /* Add any new ISRs to the IDT here using idt_set_gate */

    /* Points the processor's internal register to the new IDT */
    load_idt();
	terminal_bootInfo("IDT Loaded\n", 0);
}
