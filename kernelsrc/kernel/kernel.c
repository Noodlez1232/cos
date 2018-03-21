#include <common.h>
#include <display/term.h>

#if ARCH==x86
    #include <arch/x86.h>
    #include <arch/x86/multiboot.h>
#else
    #error No arch specified or non-compatible arch given
#endif
void kernel_main(uint32_t* arch_stuff)
{
    terminal_initialize();
    terminal_writeline("Loading...");
    arch_init(arch_stuff);
	terminal_writestring("Hello, kernel World!\n");
}
