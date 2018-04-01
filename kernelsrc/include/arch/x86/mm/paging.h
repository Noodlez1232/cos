#ifndef PAGING_H
#define PAGING_H 1
#include <common.h>

enum pgflags
{
    P =  0x0,
    RW = 0x1,
    US = 0x2,
    PWT = 0x4,
    PCD = 0x8,
    A = 0x10,
    D = 0x80,
    PS = 0x80,
    PAT = 0x100,
    G = 0x200
};

uint32_t set_pte(uint32_t addr, uint16_t flags);

uint32_t set_pde(uint32_t addr, uint16_t flags);

void load_pd(uint32_t physaddr);

#endif
