#ifndef PIT_H
#define PIT_H 1

#include <common.h>
#include <arch/x86/regs.h>
#include <arch/x86/irqs.h>

volatile bool pit_installed;

typedef void (*timer_handler_t) (uint32_t current_ticks);

void pit_install(uint32_t frequency, isr_t handler);
void pit_phase(uint32_t hz);
void pit_uninstall();

#endif

