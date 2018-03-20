#ifndef IDT_H
#define IDT_H 1

#include <common.h>

#define IDT_SIZE 128

/* Defines an IDT entry */
struct idt_entry
{
    uint16_t base_low;
    uint16_t sel;        /* Our kernel segment goes here! */
    uint8_t always0;     /* This will ALWAYS be set to 0! */
    uint8_t flags;       /* Set using the above table! */
    uint16_t base_high;
} __attribute__((packed)); //Make sure it aligns to 4 bytes
typedef struct idt_entry idt_entry_t;

struct idt_ptr
{
    uint16_t limit;
    uint32_t* base;
} __attribute__((packed));
typedef struct idt_ptr idt_ptr_t;

void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags);

void init_idt();

extern void load_idt();

#endif
