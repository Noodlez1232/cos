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
#include <sys/memory/pmm.h> //Can't use the PMM yet
//Our interrupt stuffs
#include <sys/isrs.h>
#include <sys/irqs.h>
#include <sys/timer.h>
#include <sys/syscall.h>
//Our keyboard stuffs
#include <input/keyboard.h>
//Our input stuffs
#include <input/term.h>
#include <input/terminalcommands.h>
#include <sys/serial.h>

//TODO: Fix the damn timer. I just don't have the time for that, though.

void kernel_main(multiboot_info_t* mbt, uint32_t magic)
{
	terminal_initialize(); //Init our terminal
	terminal_info("Initalizing COS\n", 2); //Tell the user that we are initalizing the OS now
	//We check to make sure the OS was loaded by a multiboot bootloader
	if (magic != MULTIBOOT_BOOTLOADER_MAGIC)
	{
		//If not, we tell the user that that will not do
		terminal_info("Kernel not booted with a multiboot compliant bootloader!\n", 1);
		terminal_writeline("The kernel will now halt.");
		halt();
	}
	init_gdt(); //Init our stupid GDT
	init_idt(); //Init our amazing IDT
	isrs_install(); //Init our ISRs
	irq_install(); //Init our IRQs
	keyboard_install(); //Init the keyboard
	serial_init(); //Init the serial port
	pmm_init(mbt); //Initalize our physical memory manager
#if 0
	terminal_writeline("Testing kmalloc");
	uint32_t* hello;
	hello = (uint32_t*) kmalloc(1024);
	hello[5] = 3;
	terminal_writehexdword((uint32_t) hello);
#endif
	//AFTER THIS POINT ON, THE MULTIBOOT INFO TABLE WILL NOT BE AVAILABLE
	//(insert paging enabler here)
	//We inform the user that everyting went well
	terminal_info("COS Initalized\n", 0);
	//We welcome the user
	terminal_writeline("Welcome to crappy os!");
	//Then we inform the user that we will be handing over control to the terminal
	terminal_writeline("Handing over control to the terminal");
	//TODO: Make a better way to do this
	terminal_inputinitalize();
	while (1)
	{
		terminal_tick();
	}
}