#ifndef KMALLOC_H
#define KMALLOC_H 1
#include <common.h>

uint32_t kmalloc_a(size_t sz);  // page aligned.
uint32_t kmalloc_p(size_t sz, u32int *phys); // returns a physical address.
uint32_t kmalloc_ap(size_t sz, u32int *phys); // page aligned and returns a physical address.
uint32_t kmalloc(size_t sz); // vanilla (normal). 


#endif