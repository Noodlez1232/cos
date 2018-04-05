#include <common.h>
#include <arch/x86.h>
#include <arch/x86/gdt.h>
#include <arch/x86/interrupts.h>
#include <arch/x86/port.h>
#include <arch/x86/regs.h>
#include <arch/x86/idt.h>
#include <arch/x86/isrs.h>
#include <arch/x86/irqs.h>
#include <arch/x86/pit.h>
#include <arch/x86/mm/paging.h>
#include <arch/x86/mm/physmm.h>
#include <arch/x86/multiboot.h>

#include <display/term.h>

uint32_t timer_ticks;
bool timer_enabled;

uint32_t pt1[1024] __attribute__ ((aligned (4096)));
uint32_t pt2[1024] __attribute__ ((aligned (4096)));
uint32_t pd1[1024] __attribute__ ((aligned (4096)));

void set_paging();
void set_pmm_blocks(multiboot_info_t* grub_table);

//Our bitmap to use. This is 32KB or for the full 4GB address space
uint32_t bitmap[0x7FFF];

extern uint32_t _kernel_start;

void arch_init(arch_info_table_t* arch_stuff)
{

    //Let's first check to make sure that the kernel was booted with a multiboot compliant bootloader
    if (arch_stuff->multiboot_magic != 0x2BADB002)
    {
        terminal_set_color(VGA_COLOR_RED, VGA_COLOR_WHITE);
        terminal_writeline("COS must be loaded using a multiboot compiant loader (such as GRUB)!");
        terminal_writeline("Halting");
        halt();
    }

    init_gdt();
    init_idt();
    init_isrs();
    init_irqs();
    set_paging();
    //Now that paging is set up, we can set the terminal buffer to the mapped address
    terminal_buffer = 0xC03FF000;

    //And now we initialize our physical memory manager.
    pmm_init(bitmap, 0x7FFF);
    set_pmm_blocks(arch_stuff->mbt);

//     pit_install(1000, &x86_pit_handler);
//     timer_enabled = false;
//     timer_ticks = 0;
    terminal_writestring("Arch inited\n");
}

void set_paging()
{
    uint32_t pt1_phys = (uint32_t)pt1 - KERNEL_BASE;
    uint32_t pt2_phys = (uint32_t)pt2 - KERNEL_BASE;
    uint32_t pd1_phys = (uint32_t)pd1 - KERNEL_BASE;
	terminal_debug_writeline("PT");
	terminal_debug_writehexdword(pt1);
	terminal_debug_putchar('\n');
	terminal_debug_writeline("PD");
	terminal_debug_writehexdword(pd1);
    terminal_debug_putchar('\n');
    terminal_debug_writeline("PTP");
    terminal_debug_writehexdword(pt1_phys);
    terminal_debug_putchar('\n');
    terminal_debug_writeline("PDP");
    terminal_debug_writehexdword(pd1_phys);
    terminal_debug_putchar('\n');
    terminal_debug_writeline("KB");
    terminal_debug_writehexdword(KERNEL_BASE);
    terminal_debug_putchar('\n');
    //First, let's set all the tables to 0
    for (uint32_t i = 0; i < 1024; i++)
    {
        pt1[i] = 0;
        pt2[i] = 0;
        pd1[i] = 0;
    }

    //Set all of them to present and read write
    uint16_t flags = P | RW;
    for (uint32_t i = 0; i<1024; i++)
    {
        //Map the first physical 4MB to the table
        pt1[i] = set_pte(i * 4096, flags);
        //Map the next physical 4MB to the table
        pt2[i] = set_pte(i * 4096 + (4096 * 1024), flags);
    }
    //Map the VGA driver to the final mapping
    pt1[1023] = set_pte(0xB8000, flags);
    //And we set up our page table
    pd1[KERNEL_BASE / 0x400000] = set_pde(pt1_phys, flags);
    pd1[KERNEL_BASE / 0x400000 + 1] = set_pde(pt2_phys, flags);
    load_pd(pd1_phys);
}

//We set the certain parts that we will be using in our kernel to used.
void set_pmm_blocks(multiboot_info_t* grub_table)
{
    //We set a temporary address for the grub table to sit in, from which we get the memory map, which we map a full 8kb area
    pt2[1023] = set_pte((uint32_t)grub_table, P);
    grub_table = (multiboot_info_t*) (((uint32_t)grub_table % 4096) + KERNEL_BASE + 0x7FF000);
    terminal_debug_writeline("Grub table init");
    terminal_debug_writehexdword((uint32_t)grub_table);
    terminal_debug_putchar('\n');
    //Make sure the system knows that this is there
    tlb_flush();
    //And now we get the address of the memory map
    multiboot_memory_map_t* memmap = (multiboot_memory_map_t*)  grub_table -> mmap_addr;
    uint32_t memmap_length = grub_table -> mmap_length;
    terminal_writeline("Get mmap");
    //And now we set that up in our page table
    pt2[1022] = set_pte((uint32_t)memmap, P);
    pt2[1022] = set_pte((uint32_t)(memmap + 4096), P);
    //And make sure the system updates that change
    tlb_flush();
    //And now we init the pmm blocks with that memory table
    phys_parsememtable((multiboot_memory_map_t*)KERNEL_BASE + 0x7FE000, memmap_length);
}

void delay(uint32_t ms)
{
    timer_enabled = true;
    while (timer_ticks < ms);
    timer_enabled = false;
}



void x86_pit_handler(regs_t* r)
{
    if (timer_enabled)
    {
        timer_ticks++;
//         terminal_writehexdword(timer_ticks);
    }
    send_EOI(0);
}
