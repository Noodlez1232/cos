#ifndef PHYSMM_H
#define PHYSMM_H 1
#include <common.h>

uint32_t* phys_allocatemem(uint32_t size);
void phys_setmemused(uint32_t block);
void phys_setmemfree(uint32_t block);
uint32_t phys_findfreemem(uint32_t size);
void phys_deallocatemem(uint32_t* pointer);
uint32_t phys_getfreemem();
void phys_parsememtable(uint32_t* grub_memtable);
uint32_t pmm_init(uint32_t* bitmap, uint32_t bitmap_size);

#endif
