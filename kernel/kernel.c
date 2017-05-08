/* COS kernel main c file thing that does the other things for the thing */


//Our required includes
#include <stddef.h>
#include <stdint.h>
//Our common stuffs
#include <common.h>
//Our TTY and basic terminal functions
#include <display/term.h>
//Our descriptor tables
#include <sys/gdt.h>
#include <sys/idt.h>
//Our interrupt stuffs
#include <sys/isrs.h>
#include <sys/irqs.h>
#include <sys/timer.h>
 
 
 
//Make sure it's only compiling under a cross-compiler
#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif
 
//This OS will only compile for i686-elf
#if !defined(__i386__)
#error "This OS needs to be compiled with a ix86-elf compiler, so get one, please"
#endif


void kernel_main(void)
{
	uint8_t derp = 5;
	/* Initialize terminal interface */
	terminal_initialize();
	terminal_bootInfo("Initalizing GDT\n", 2);
	init_gdt();			//Init our stupid GDT
	terminal_bootInfo("Initalizing IDT\n", 2);
	init_idt();			//init our amazing IDT
	terminal_bootInfo("Initalizing ISRs\n", 2);
	isrs_install();		//Init our ISRs
	terminal_bootInfo("Initalizing IRQs\n", 2);
	irq_install();		//Init our IRQs
	timer_install();	//Init the timer
	terminal_writestring("System initalized! Welcome to crappy os!\n");
	terminal_writeline("Gonna wait for 5 seconds");
	timer_wait(5*18);
	terminal_writeline("Thanks for waiting, now let's get the show started!");
	halt_with_interrupts();
	termial_writeline("wat?");
	halt();
}
