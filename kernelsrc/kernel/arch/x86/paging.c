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

uint32_t set_pte(uint32_t addr, uint16_t flags)
{
    //First we trim the address of the pte
    addr = addr & 0xFFFFF000;
    //now we trim the flags
    flags = flags & 0xFFF;
    return addr & flags;
}
