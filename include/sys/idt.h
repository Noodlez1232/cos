#ifndef IDT_H
#define IDT_H 1

#include <common.h>

/* Defines an IDT entry */
struct idt_entry
{
    uint16_t base_low;
    uint16_t sel;        /* Our kernel segment goes here! */
    uint8_t always0;     /* This will ALWAYS be set to 0! */
    uint8_t flags;       /* Set using the above table! */
    uint16_t base_high;
} __attribute__((packed)); //Make sure it aligns to 4 bytes

struct idt_ptr
{
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

//Declare our IDT entries of 256

//Even though only 32 will be used for now, the rest act as a sort of trap so that there's no
//sort of funny stuff happening if somebody calls an undefined IRQ
struct idt_entry idt[256];
struct idt_ptr idtp;

void idt_set_gate(unsigned char num, unsigned long base, unsigned short sel, unsigned char flags);

void init_idt();

extern void load_idt();

#endif