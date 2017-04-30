#include <stddef.h>
#include <stdint.h>
#include <kernel/mem.h>
#include <display/term.h>
 
/* Check if the compiler thinks we are targeting the wrong operating system. */
#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif
 
/* This tutorial will only work for the 32-bit ix86 targets. */
#if !defined(__i386__)
#error "This tutorial needs to be compiled with a ix86-elf compiler"
#endif


void kernel_main(void)
{
	/* Initialize terminal interface */
	terminal_initialize();
	terminal_writestring("Hello World!\nThis is just a friendly scroll test");
	terminal_setcursor(0,23);
	terminal_writestring("Testscroll\ntest\ntest2");
}