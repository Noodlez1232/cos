#ifndef TIMER_H
#define TIMER_H 1

#include <common.h>

volatile bool timer_installed;

typedef void (*timer_handler_t) (uint32_t current_ticks);

void timer_install(uint32_t frequency);
void timer_handler(regs_t *r);
void timer_wait(unsigned int ticks);
void timer_phase(uint32_t hz);
void timer_reset();
void timer_uninstall();
void timer_delay(uint32_t ms);

#endif