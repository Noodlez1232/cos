#include <common.h>
#include <arch/x86/port.h>

//Takes input from an IO port
uint8_t inportb(uint16_t _port)
{
	uint8_t rv;
	asm volatile("inb %1, %0" : "=a" (rv) : "dN" (_port));
	return rv;
}

//Outputs to an IO port
void outportb(uint16_t _port, uint8_t _data)
{
	asm volatile("outb %1, %0" : : "dN" (_port), "a" (_data));
}

uint16_t inportw(uint16_t _port)
{
	uint16_t rv;
	asm volatile("inw %1, %0" : "=a" (rv) : "dN" (_port));
	return rv;
}

void outportw(uint16_t _port, uint16_t _data)
{
	asm volatile("outw %1, %0" : : "dN" (_port), "a" (_data));
}
