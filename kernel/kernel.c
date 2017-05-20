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
	/* Initialize terminal interface */
	terminal_initialize();
	init_gdt();			//Init our stupid GDT
	init_idt();			//init our amazing IDT
	isrs_install();		//Init our ISRs
	irq_install();		//Init our IRQs
	timer_install();	//Init the timer
	terminal_writestring("System initalized! Welcome to crappy os!\n");
	terminal_writeline("Gonna wait for 5 seconds");
	timer_wait(5);
	terminal_writeline("Thanks for waiting, now let's get the show started!");
	timer_uninstall();
	halt();
}
