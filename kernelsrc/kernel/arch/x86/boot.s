extern _kernel_start

;Note: There are some comments still left over from the original barebones. These are distinguished because
;the person that wrote them puts spaces after their comment tag, I don't. So that's an easy way to distinguish them

; Declare constants for the multiboot header.
MBALIGN  equ  1<<0              ; align loaded modules on page boundaries
MEMINFO  equ  1<<1              ; provide memory map
FLAGS    equ  MBALIGN | MEMINFO ; this is the Multiboot 'flag' field
MAGIC    equ  0x1BADB002        ; 'magic number' lets bootloader find the header
CHECKSUM equ -(MAGIC + FLAGS)   ; checksum of above, to prove we are multiboot



section .multiboot
align 4
	dd MAGIC
	dd FLAGS
	dd CHECKSUM
; Declare a multiboot header that marks the program as a kernel. These are magic
; values that are documented in the multiboot standard. The bootloader will
; search for this signature in the first 8 KiB of the kernel file, aligned at a
; 32-bit boundary. The signature is in its own section so the header can be
; forced to be within the first 8 KiB of the kernel file.

KERNEL_VIRTUAL_BASE equ 0xC0000000
KERNEL_PAGE_NUMBER equ (KERNEL_VIRTUAL_BASE >> 22);This is the entry which we want for our page (The base divided by 4MiB)

KERNEL_BASE equ (_kernel_start - KERNEL_VIRTUAL_BASE)

; The multiboot standard does not define the value of the stack pointer register
; (esp) and it is up to the kernel to provide a stack. This allocates room for a
; small stack by creating a symbol at the bottom of it, then allocating 16384
; bytes for it, and finally creating a symbol at the top. The stack grows
; downwards on x86. The stack is in its own section so it can be marked nobits,
; which means the kernel file is smaller because it does not contain an
; uninitialized stack. The stack on x86 must be 16-byte aligned according to the
; System V ABI standard and de-facto extensions. The compiler will assume the
; stack is properly aligned and failure to align the stack will result in
; undefined behavior.
section .bss
align 16
stack_bottom:
resb 16384 ; 16 KiB
stack_top:
align 4096
arch_data:
    resb 256
;We need to define where we are going to put our paging stuff
;They must be alligned by 4kib
section .data
align 4096
BootPageDirectory:
    ; This page directory entry identity-maps the first 4MB of the 32-bit physical address space.
    ; All bits are clear except the following:
    ; bit 7: PS The kernel page is 4MB.
    ; bit 1: RW The kernel page is read/write.
    ; bit 0: P  The kernel page is present.
    ; This entry must be here -- otherwise the kernel will crash immediately after paging is
    ; enabled because it can't fetch the next instruction! It's ok to unmap this page later.
    dd 0x00000083
    times (KERNEL_PAGE_NUMBER - 1) dd 0                 ; Pages before kernel space.
    ; This page directory entry defines a 4MB page containing the kernel.
    dd 0x00000083
    times (1024 - KERNEL_PAGE_NUMBER - 1) dd 0  ; Pages after the kernel image.

; The linker script specifies _start as the entry point to the kernel and the
; bootloader will jump to this position once the kernel has been loaded. It
; doesn't make sense to return from this function as the bootloader is gone.
; Declare _start as a function symbol with the given symbol size.
section .text
global _start
_start:
; NOTE: Until paging is set up, the code must be position-independent and use physical
; addresses, not virtual ones!
mov ecx, (BootPageDirectory - KERNEL_VIRTUAL_BASE)
mov cr3, ecx                                        ; Load Page Directory Base Register.

mov ecx, cr4
or ecx, 0x00000010                          ; Set PSE bit in CR4 to enable 4MB pages.
mov cr4, ecx

mov ecx, cr0
or ecx, 0x80000000                          ; Set PG bit in CR0 to enable paging.
mov cr0, ecx

; Start fetching instructions in kernel space.
; Since eip at this point holds the physical address of this command (approximately 0x00100000)
; we need to do a long jump to the correct virtual address of StartInHigherHalf which is
; approximately 0xC0100000.
mov ecx, eax
lea eax, [HigherHalfStart]
jmp eax

HigherHalfStart:
    ;Finally into the higher half. No more dealing with that stupid crap with
    ;the virtual base and stuff. Let's get rid of that identity page

    mov dword [BootPageDirectory], 0 ;We need to specify DWORD so it clears the whole entry
    invlpg [0] ;And we invalidate that page

	mov eax, ecx
	mov esp, stack_top


	mov [arch_data], eax
	mov [(arch_data + 4)], ebx
	lea ecx, [BootPageDirectory]
	mov [(arch_data + 8)], ecx
	push arch_data


	extern kernel_main
	call kernel_main

	cli
.hang:	hlt
	jmp .hang
.end:
