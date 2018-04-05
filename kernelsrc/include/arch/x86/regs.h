#ifndef REGS_H
#define REGS_H 1

/* This defines what the stack looks like after an ISR was running */
// typedef struct regs
// {
//    unsigned int gs, fs, es, ds;                  		// Data segment selector
//    unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax; // Pushed by pusha.
//    unsigned int int_no, err_code;    					// Interrupt number and error code (if applicable)
//    unsigned int eip, cs, eflags, useresp, ss; 			// Pushed by the processor automatically.
// } regs_t;

typedef struct regs
{
    uint32_t ds;                  // Data segment selector
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax; // Pushed by pusha.
    uint32_t int_no, err_code;    // Interrupt number and error code (if applicable)
    uint32_t eip, cs, eflags, useresp, ss; // Pushed by the processor automatically.
} regs_t;

#endif
