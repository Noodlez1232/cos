#include <arch/x86/paging.h>

uint16_t set_pte_flags(bool p, bool rw, bool us, bool pwt, bool pcd, bool a, bool d, bool pat, bool g)
{
    uint16_t ret = 0;
    if (p) 
        ret = ret | 0x1;
    if (rw)
        ret = ret | 0x2;
    if (us)
        ret = ret | 0x4;
    if (pwt)
        ret = ret | 0x8;
    if (pwt)
        ret = ret | 0x10;
    if (pcd)
        ret = ret | 0x20;
    if (a)
        ret = ret | 0x40;
    if (d)
        ret = ret | 0x80;
    if (pat)
        ret = ret | 0x100;
    if (g)
        ret = ret | 0x200;
    return ret;
}

uint8_t set_pde_flags(bool p, bool rw, bool us, bool pwt, bool pcd, bool a, bool ps)
{
    uint16_t ret = 0;
    if (p) 
        ret = ret | 0x1;
    if (rw)
        ret = ret | 0x2;
    if (us)
        ret = ret | 0x4;
    if (pwt)
        ret = ret | 0x8;
    if (pwt)
        ret = ret | 0x10;
    if (pcd)
        ret = ret | 0x20;
    if (a)
        ret = ret | 0x40;
    if (ps)
        ret = ret | 0x100;
    return ret;
}

uint32_t set_pte(uint32_t addr, uint16_t flags)
{
    //First we trim the address of the pte
    addr = addr & 0xFFFFF000;
    //now we trim the flags
    flags = flags & 0xFFF;
    return addr & flags;
}

uint32_t set_pde(uint32_t addr, uint8_t flags)
{
    //First we trim the address of the pte
    addr = addr & 0xFFFFF000;
    //now we trim the flags
    flags = flags & 0xFF;
    return addr & flags;
}

void load_pde(uint32_t physaddr)
{
    asm volatile("movl %0, %%cr3" : : "r"(physaddr) : ); 
}

void * get_physaddr(void * virtualaddr)
{
    unsigned long pdindex = (unsigned long)virtualaddr >> 22;
    unsigned long ptindex = (unsigned long)virtualaddr >> 12 & 0x03FF;

    unsigned long * pt = ((unsigned long *)0xFFC00000) + (0x400 * pdindex);
    // Here you need to check whether the PT entry is present.
 
    return (void *)((pt[ptindex] & ~0xFFF) + ((unsigned long)virtualaddr & 0xFFF));
}
