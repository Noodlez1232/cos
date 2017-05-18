#ifndef _COMMON_H
#define _COMMON_H 1

#define DEBUG 1

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

int memcmp(const void* aptr, const void* bptr, size_t size);
void* memcpy(void* restrict dstptr, const void* restrict srcptr, size_t size);
void* memmove(void* dstptr, const void* srcptr, size_t size);
void* memset(void* bufptr, int value, size_t size);
size_t strlen(const char* str);
void halt();
void halt_with_interrupts();
void outportb(unsigned short _port, unsigned char _data);
unsigned char inportb(unsigned short _port);




/* This defines what the stack looks like after an ISR was running */
struct regs
{
   unsigned int gs, fs, es, ds;                  // Data segment selector
   unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax; // Pushed by pusha.
   unsigned int int_no, err_code;    // Interrupt number and error code (if applicable)
   unsigned int eip, cs, eflags, useresp, ss; // Pushed by the processor automatically.
};

#endif
