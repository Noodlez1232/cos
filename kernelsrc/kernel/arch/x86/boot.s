extern _kernel_start

;Note: There are some comments still left over from the original barebones. These are distinguished because
;the person that wrote them puts spaces after their comment tag, I don't. So that's an easy way to distinguish them

; Declare constants for the multiboot header.
MBALIGN  equ  1<<0              ; align loaded modules on page boundaries
MEMINFO  equ  1<<1              ; provide memory map
FLAGS    equ  MBALIGN | MEMINFO ; this is the Multiboot 'flag' field
MAGIC    equ  0x1BADB002        ; 'magic number' lets bootloader find the header
CHECKSUM equ -(MAGIC + FLAGS)   ; checksum of above, to prove we are multiboot

; Declare a multiboot header that marks the program as a kernel. These are magic
; values that are documented in the multiboot standard. The bootloader will
; search for this signature in the first 8 KiB of the kernel file, aligned at a
; 32-bit boundary. The signature is in its own section so the header can be
; forced to be within the first 8 KiB of the kernel file.


KERNEL_VIRTUAL_BASE equ 0xC0000000
KERNEL_PAGE equ KERNEL_VIRTUAL_BASE >> 22

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
;We need to define where we are going to put our paging stuff
;They must be alligned by 4kib
align 4096
BootPageDirectory:
    resb 4096
;We use this to identity map the kernel
BootPageTable1:
    resb 4096
;We use this to map the kernel up to the virtual base
BootPageTable2:
    resb 4096
arch_data:
    resb 256

; The linker script specifies _start as the entry point to the kernel and the
; bootloader will jump to this position once the kernel has been loaded. It
; doesn't make sense to return from this function as the bootloader is gone.
; Declare _start as a function symbol with the given symbol size.
section .text
align 4
	dd MAGIC
	dd FLAGS
	dd CHECKSUM
global _start
_start:

    ;The first thing we need to do is identity map the kernel
    ;This helps everything when we jump to the kernel

    ;We need to map the first page table to the directory
    ;In order to do this, we must first set up the entry

    ;Load up the page1 into ebx
    mov ebx, (BootPageTable1 - KERNEL_VIRTUAL_BASE)

    ;Now we align the address of the page table and free up that bit for flags in one swoop
    and ebx, 0xFFFFF000
    ;And we set its flags. This will be present and read/write.
    or ebx, 0x2
    ;And now we put this into the directory at index 0
    mov [BootPageDirectory], ebx

    ;And now we set up the page table. This should be easy enough. We just loop through 4kb starting at the kernel base
    mov eax, 0x0
    mov ebx, _kernel_start - KERNEL_VIRTUAL_BASE
    .loopFillTable1:
        ;Now we align the address of the page table and free up that bit for flags in one swoop
        and ebx, 0xFFFFF000
        ;And we set its flags. This will be present and read/write.
        or ebx, 0x2
        ;Then we store this at the page entry that we want
        mov [BootPageTable1+eax*4], ebx
        ;This is our counter
        inc eax
        ;And we set our ebx (our pointer to where we are in the kernel) to point at the next area to map
        add eax, 4096
        ;And now we check to see if we are done with that table
        cmp eax, 1024
        jne .loopFillTable1

    ;And now we map our virtual kernel
    ;We can start by mapping the second page table to the directory's second entry
    mov ebx, (BootPageTable2 - KERNEL_VIRTUAL_BASE)
    ;Now we align the address of the page table and free up that bit for flags in one swoop
    and ebx, 0xFFFFF000
    ;And we set its flags. This will be present and read/write.
    or ebx, 0x2
    ;And now we store that table at the entry we calculated earlier
    mov [BootPageDirectory + (4 * KERNEL_PAGE)], ebx

    ;And now we do the same thing with the virtual allocation
    mov eax, 0x0
    mov ebx, _kernel_start - KERNEL_VIRTUAL_BASE
    .loopFillTable2:
        ;Now we align the address of the page table and free up that bit for flags in one swoop
        and ebx, 0xFFFFF000
        ;And we set its flags. This will be present and read/write.
        or ebx, 0x2
        ;Then we store this at the page entry that we want
        mov [BootPageTable2+eax*4], ebx
        ;This is our counter
        inc eax
        ;And we set our ebx (our pointer to where we are in the kernel) to point at the next area to map
        add eax, 4096
        ;And now we check to see if we are done with that table
        cmp eax, 1024
        jne .loopFillTable2

    ;I want the VGA area to be at the end of the kernel area, actually, so I'm gonna set it as the last entry of the virtual kernel


    ;This is stupid to do but I'm going to do it anyways
    ;We assume the machine supports PSE and we enable it.
    ;Load the page directory into control register 3
    mov ecx, (BootPageDirectory - KERNEL_VIRTUAL_BASE)
    mov cr3, ecx
    ;And now we enable PSE
    ;We can't work on cr4 itself, so we move it into a temp register to work on
    mov ecx, cr4
    or ecx, 0x10
    mov cr4, ecx
    ;And now we enable paging.
    mov ecx, cr0
    or ecx, 0x80000000
    mov cr0, ecx

    ;Now we have paging enabled. Luckily, we identity mapped the kernel, so we can still run, but
    ;we do want to move to the upper half, Luckily we have something for that.
    ;All labels are linked at the virtual memory, so all we need to do is load the label's address
    ;into a general purpose register, and jump to it (absolute jump). It's as simple as that
    lea ecx, [HigherHalfStart]
    jmp ecx

HigherHalfStart:
    ;And now we're in the higher half. Cool, yeah? So now we can remove the identity map.
    ;This is quite simple. All we need to do is just remove that entry in the boot directory
    ;One thing you will notice is that we don't have to subtract the virtual base because we aren't in the lower half anymore. It's nice.
    ;mov dword [BootPageDirectory], 0 ;We need to specify dword so that it doesn't just 0 one byte but all four bytes
    ;And we invalidate that page table
    ;invlpg [0]

	; To set up a stack, we set the esp register to point to the top of our
	; stack (as it grows downwards on x86 systems). This is necessarily done
	; in assembly as languages such as C cannot function without a stack.
	mov esp, stack_top

	; This is a good place to initialize crucial processor state before the
	; high-level kernel is entered. It's best to minimize the early
	; environment where crucial features are offline. Note that the
	; processor is not fully initialized yet: Features such as floating
	; point instructions and instruction set extensions are not initialized
	; yet. The GDT should be loaded here. Paging should be enabled here.
	; C++ features such as global constructors and exceptions will require
	; runtime support to work as well.

	; Enter the high-level kernel. The ABI requires the stack is 16-byte
	; aligned at the time of the call instruction (which afterwards pushes
	; the return pointer of size 4 bytes). The stack was originally 16-byte
	; aligned above and we've since pushed a multiple of 16 bytes to the
	; stack since (pushed 0 bytes so far) and the alignment is thus
	; preserved and the call is well defined.
        ; note, that if you are building on Windows, C functions may have "_" prefix in assembly: _kernel_main
	extern kernel_main
	call kernel_main

	; If the system has nothing more to do, put the computer into an
	; infinite loop. To do that:
	; 1) Disable interrupts with cli (clear interrupt enable in eflags).
	;    They are already disabled by the bootloader, so this is not needed.
	;    Mind that you might later enable interrupts and return from
	;    kernel_main (which is sort of nonsensical to do).
	; 2) Wait for the next interrupt to arrive with hlt (halt instruction).
	;    Since they are disabled, this will lock up the computer.
	; 3) Jump to the hlt instruction if it ever wakes up due to a
	;    non-maskable interrupt occurring or due to system management mode.
	cli
.hang:	hlt
	jmp .hang
.end:
