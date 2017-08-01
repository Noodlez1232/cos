#ifndef _COMMON_H
#define _COMMON_H 1

#define DEBUG 1

//0=None, for debugging, use COM1
//1=Bochs, for debugging, use the Bochs debugger
//2=QEMU, for debugging, use GDB (When implemented)
//3=Other, for debugging, use COM1
#define VM 3

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>

bool strcmp(char* aptr, char* bptr);
int memcmp(const void* aptr, const void* bptr, size_t size);
void* memcpy(void* restrict dstptr, const void* restrict srcptr, size_t size);
void* memmove(void* dstptr, const void* srcptr, size_t size);
void* memset(void* bufptr, int value, size_t size);
size_t strlen(const char* str);
void halt();
void halt_with_interrupts();
void clear_interrupts();
void set_interrupts();
void outportb(unsigned short _port, unsigned char _data);
unsigned char inportb(unsigned short _port);
uint16_t inportw(unsigned short _port);
void outportw(unsigned short _port, unsigned short _data);

//outputs a character to the debug console
#define BochsConsolePrintChar(c) outportb(0xe9, c)
//stops simulation and breaks into the debug console
#define BochsBreak() outportw(0x8A00,0x8A00); outportw(0x8A00,0x08AE0);



/* This defines what the stack looks like after an ISR was running */
typedef struct regs
{
   unsigned int gs, fs, es, ds;                  		// Data segment selector
   unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax; // Pushed by pusha.
   unsigned int int_no, err_code;    					// Interrupt number and error code (if applicable)
   unsigned int eip, cs, eflags, useresp, ss; 			// Pushed by the processor automatically.
} regs_t;

#endif
