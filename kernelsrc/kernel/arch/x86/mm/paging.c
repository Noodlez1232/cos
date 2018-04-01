#include <arch/x86/mm/paging.h>

uint32_t set_pte(uint32_t addr, uint16_t flags)
{
    //First we trim the address of the pte
    addr = addr & 0xFFFFF000;
    //now we trim the flags
    flags = flags & 0xFFF;
    return addr | flags;
}

uint32_t set_pde(uint32_t addr, uint16_t flags)
{
    //First we trim the address of the pte
    addr = addr & 0xFFFFF000;
    //now we trim the flags
    flags = flags & 0xFFF;
    return addr | flags;
}

void load_pd(uint32_t physaddr)
{
    asm volatile("xchg %bx, %bx");
    asm volatile("movl %0, %%cr3" : : "r"(physaddr) : );
}

// //Map a physical address to a virtual address.
// void map_address(phys_addr_t physaddr, virt_addr_t virtaddr)
// {
//
// }
