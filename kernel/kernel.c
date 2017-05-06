#include <stddef.h>
#include <stdint.h>
#include <common.h>
#include <display/term.h>
#include <sys/gdt.h>
#include <sys/idt.h>
 
//Make sure it's only compiling under a cross-compiler
#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif
 
//This OS will only compile for i686-elf
#if !defined(__i386__)
#error "This tutorial needs to be compiled with a ix86-elf compiler"
#endif


void kernel_main(void)
{
	uint8_t derp = 5;
	/* Initialize terminal interface */
	terminal_initialize();
	terminal_bootInfo("Initalizing GDT\n", 2);
	init_gdt();		//Init our stupid GDT
	terminal_bootInfo("Initalizing IDT\n", 2);
	init_idt();		//init our amazing IDT
	terminal_bootInfo("Initalizing ISRs\n", 2);
	isrs_install();	//Init our ISRs
	terminal_writestring("System initalized! Welcome to crappy os!\n");
	terminal_writestring("This is a test of the ISRs");
	asm volatile ("int $0x3");
}
