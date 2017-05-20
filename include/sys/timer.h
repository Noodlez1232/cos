#ifndef TIMER_H
#define TIMER_H 1

#include <common.h>

void timer_install();
void timer_handler(regs_t *r);
void timer_wait(unsigned int ticks);
void timer_phase(int hz);
void timer_reset();
void timer_uninstall();

#endif