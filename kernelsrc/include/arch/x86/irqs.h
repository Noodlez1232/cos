#ifndef IRQS_H
#define IRQS_H 1

#include <common.h>
#include <arch/x86/regs.h>
#include <arch/x86/isrs.h>

extern void irq0();
extern void irq1();
extern void irq2();
extern void irq3();
extern void irq4();
extern void irq5();
extern void irq6();
extern void irq7();
extern void irq8();
extern void irq9();
extern void irq10();
extern void irq11();
extern void irq12();
extern void irq13();
extern void irq14();
extern void irq15();

#if 0
#define IRQ0 32
#define IRQ1 33
#define IRQ2 34
#define IRQ3 35
#define IRQ4 36
#define IRQ5 37
#define IRQ6 38
#define IRQ7 39
#define IRQ8 40
#define IRQ9 41
#define IRQ10 42
#define IRQ11 43
#define IRQ12 44
#define IRQ13 45
#define IRQ14 46
#define IRQ15 47
#endif


void irq_remap(void);

void irq_uninstall_handler(uint8_t irq);
void irq_install_handler(uint8_t irq, isr_t handler);
void init_irqs(void);
void irq_handler(regs_t *r);

void master_EOI();
void slave_EOI();
void send_EOI(uint8_t irq_no);

#endif

