#include <common.h>
#include <display/term.h>

#if ARCH==x86
#include <arch/x86.h>
#endif
 
void kernel_main()
{
	terminal_initialize();
    
    //Initialize everything arch-specific
    arch_init();
    delay(1000);
    
	terminal_writestring("Hello, kernel World!\n");
}
