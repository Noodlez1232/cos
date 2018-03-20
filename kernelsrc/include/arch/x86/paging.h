#ifndef PAGING_H
#define PAGING_H 1
#include <common.h>

uint16_t set_pte_flags(bool p, bool rw, bool us, bool pwt, bool pcd, bool a, bool d, bool pat, bool g);
uint8_t set_pde_flags(bool p, bool rw, bool us, bool pwt, bool pcd, bool a, bool ps);

uint32_t set_pte(uint32_t addr, uint16_t flags);

uint32_t set_pde(uint32_t addr, uint8_t flags);
void * get_physaddr(void * virtualaddr);

#endif
