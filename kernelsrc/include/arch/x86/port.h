#ifndef X86_PORT_H
#define X86_PORT_H 1

#include <common.h>

void outportb(uint16_t _port, uint8_t _data);
uint8_t inportb(uint16_t _port);
uint16_t inportw(uint16_t _port);
void outportw(uint16_t _port, uint16_t _data);
#endif