/* 
 * File:   pagetable.h
 * Author: Nathaniel Barragan
 *
 * Created on August 4, 2017, 9:15 AM
 */

#ifndef PAGETABLE_H
#define PAGETABLE_H

#include <common.h>

typedef enum PAGETABLE_ENTRY_FLAG
{
	PTE_PRESENT = 1,			//0000000000000000000000000000001
	PTE_WRITABLE = 2,			//0000000000000000000000000000010
	PTE_USER = 4,				//0000000000000000000000000000100
	PTE_WRITETHROUGH = 8,		//0000000000000000000000000001000
	PTE_NOT_CACHEABLE = 0x10,	//0000000000000000000000000010000
	PTE_ACCESSED = 0x20,		//0000000000000000000000000100000
	PTE_DIRTY = 0x40,			//0000000000000000000000001000000
	PTE_PAT = 0x80,				//0000000000000000000000010000000
	PTE_CPU_GLOBAL = 0x100,		//0000000000000000000000100000000
	PTE_LV4_GLOBAL = 0x200,		//0000000000000000000001000000000
	PTE_FREE_BITS = 0xc00,		//0000000000000000000110000000000
	PTE_FRAME = 0x7FFFF000		//1111111111111111111000000000000
} pagetable_entry_flag_t;

typedef uint32_t pagetable_entry_t;

typedef struct pagetable
{
	pagetable_entry_t entries [1024];
} pagetable_t;

void pagetable_set_flag(pagetable_entry_t *entry, pagetable_entry_flag_t flag);
void pagetable_unset_flag(pagetable_entry_t *entry, pagetable_entry_flag_t flag);
void pagetable_set_frame_address(pagetable_entry_t *entry, phys_addr_t addr);
void pagetable_set_free_bits(pagetable_entry_t *entry, uint8_t value);



#endif /* PAGETABLE_H */

