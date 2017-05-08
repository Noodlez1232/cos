#ifndef TIMER_H
#define TIMER_H 1

#include <common.h>

void timer_install();
void timer_handler(struct regs *r);
void timer_wait(int ticks);

#endif