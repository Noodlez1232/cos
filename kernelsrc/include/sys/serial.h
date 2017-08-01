#ifndef SERIAL_H
#define SERIAL_H 1
#include <common.h>
 
void serial_init();
void serial_putchar(char c);
int is_transmit_empty();
void serial_handler(regs_t *r);

#endif