/* COS kernel main c file thing that does the other things for the thing */


//Our common stuffs
#include <common.h>
//Our TTY and basic terminal functions
#include <display/term.h>
//Our descriptor tables
#include <sys/gdt.h>
#include <sys/idt.h>
//Our memory stuffs
#include <sys/boot/multiboot.h>
//#include <sys/memory/pmm.h> //Can't use the PMM yet
//Our interrupt stuffs
#include <sys/isrs.h>
#include <sys/irqs.h>
#include <sys/timer.h>
//Our keyboard stuffs
#include <input/keyboard.h>
//Our input stuffs
#include <input/term.h>
#include <input/terminalcommands.h>


 
 //TODO: Fix the damn timer. I just don't have the time for that, though.
 
 
//Make sure it's only compiling under a cross-compiler
#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif
 
//This OS will only compile for i686-elf
#if !defined(__i386__)
#error "This OS needs to be compiled with a ix86-elf compiler, so get one, please"
#endif


void kernel_main(multiboot_info_t* mbt, unsigned int magic)
{
	terminal_initialize();	//Init our terminal
	if (magic!=MULTIBOOT_BOOTLOADER_MAGIC)
	{
		terminal_writeline("The kernel will not work properly without a multiboot loader!");
		terminal_writeline("The kernel will now halt.");
		halt();
	}
	init_gdt();				//Init our stupid GDT
	init_idt();				//init our amazing IDT
	isrs_install();			//Init our ISRs
	irq_install();			//Init our IRQs
	keyboard_install();		//Init the keyboard
	//pmm_init(mbt);				//Initalize our physical memory manager
	terminal_writeline("System initalized! Welcome to crappy os!");
	terminal_writeline("Handing over control to the terminal");
	//TODO: Make a better way to do this
	terminal_inputinitalize();
	halt_with_interrupts();
}