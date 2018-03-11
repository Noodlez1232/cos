#include <common.h>
#include <display/term.h>

#if ARCH==x86
    #include <arch/x86.h>
    #include <arch/x86/multiboot.h>
    void kernel_main(uint32_t* arch_stuff, multiboot_info_t* mbt, uint32_t magic)
    {
        terminal_initialize();
        terminal_writeline("Loading...");
        arch_init(arch_stuff, mbt, magic);
#else
    #error No arch specified or non-compatible arch given
    void kernel_main()
    {
        terminal_initialize();
#endif

	terminal_writestring("Hello, kernel World!\n");
}
