#ifndef GDT_H
#define GDT_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>


struct gdt_entry
{
   uint16_t limit_low;           //The lower 16 bits of the limit.
   uint16_t base_low;            //The lower 16 bits of the base.
   uint8_t  base_middle;         //The next 8 bits of the base.
   uint8_t  access;              //Access flags, determine what ring this segment can be used in.
   uint8_t  granularity;
   uint8_t  base_high;           //The last 8 bits of the base.
} __attribute__((packed));
typedef struct gdt_entry gdt_entry_t;

struct gdt_ptr
{
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));
typedef struct gdt_ptr gdt_ptr_t;

gdt_entry_t gdt_entries[3];		//null, code, data
gdt_ptr_t gdt_ptr;

void init_gdt();
void gdt_set_gate(int num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran);
extern void load_gdt(uint32_t);

#endif