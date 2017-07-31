#ifndef _SYSCALL_H
#define _SYSCALL_H 1

#include <common.h>

typedef struct interrupt_frame
{
    uint16_t ip;
    uint16_t cs;
    uint16_t flags;
    uint16_t sp;
    uint16_t ss;
} interrupt_frame_t;

typedef struct popa_frame
{
	uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax; // Pushed by pusha.
} popa_t;

typedef void (*syscall_t) (interrupt_frame_t int_frame, popa_t *r);

extern void syscall0();


void syscall_install();
void syscall_handler(interrupt_frame_t int_frame, popa_t *r);

#endif