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
KERNEL_PAGE equ (KERNEL_VIRTUAL_BASE / 0x400000) ;This is the entry which we want for our page (The base divided by 4MiB)

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
align 4096
BootPageTable:
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
    ;Let's start by setting up our page table. We want to allocate 1023 entries,
    ;leaving the last one for our VGA driver

    ;You will notice I'm subtracting the virtual base from every address. This
    ;is because as far as our linker knows, these things are offset by the
    ;base already, meaning we need to subtract that offset to get the physical
    ;address

    mov ecx, 1023 ; Our counter
    mov esi, (_kernel_start - KERNEL_VIRTUAL_BASE) ;Our starting point for addressing
    mov edi, (BootPageTable - KERNEL_VIRTUAL_BASE) ;Our pointer to the page table

    .FillPageTableLoop:
        mov edx, esi ;We move the address to a register to work on without affecting the counter
        and edx, 0xFFFFF000 ;We align the address by 4KiB and free up space for our flags
        or edx, 0x003 ;And we set our flags. These are RW and Present
        mov [edi], edx ;And we store that entry in our table
        add esi, 4096 ;Each entry addresses 4KiB so we add our source address by that
        add edi, 4 ;Each entry is 4 bytes long
        loop .FillPageTableLoop
    ;Now that we are done filling up our page table, let's work on mapping the VGA area
    mov edi, (BootPageTable - KERNEL_VIRTUAL_BASE + 4092)
    mov edx, 0xB8000
    or edx, 0x003
    mov [edi], edx
    ;And now we set up our directory
    ;We first identity map it. We do this to ensure that the code can continue
    ;to run even after we set up paging.
    ;We need to set up the directory entry first
    mov edx, (BootPageTable - KERNEL_VIRTUAL_BASE) ;We move our table's address into edx
    or edx, 0x003 ;And we set its flags
    ;And now we start to put it in our directory
    ;We start with the identity mapped one
    mov edi, (BootPageDirectory - KERNEL_VIRTUAL_BASE)
    mov [edi], edx
    ;And now we do the virtual one
    mov edi, (BootPageDirectory - KERNEL_VIRTUAL_BASE + (KERNEL_PAGE * 4))
    mov [edi], edx
    ;And now we start to actually enable paging
    ;We move the directory's address into cr3
    mov edx, (BootPageDirectory - KERNEL_VIRTUAL_BASE)
    mov cr3, edx
    ;And now we turn on paging.
    ;Set the PSE bit in cr4 to enable 4MB pages
    mov edx, cr4 ;Move it into a temporary register to modify it
    or edx, 0x00000010 ;Set the flag
    mov cr4, edx ;And move it back
    xchg bx,bx
    ;And now set the PG bit to enable paging
    mov edx, cr0 ;Move it into a temporary register to modify it
    or edx, 0x80000000 ;Set the flag
    mov cr0, edx ;And move it back

    ;Now paging is on. The only reason code can still run is because we identity
    ;mapped this bit of code.
    ;In order to actually start running code in the higher half, we need to jump
    ;to it.
    ;We need to do an absolute jump, not just a relative one. To do this, we load
    ;the address of the label and jump to the address in that register
    ;lea edx, [HigherHalfStart] ;We load the address
    ;jmp edx ;And we jump to it
    push dword [HigherHalfStart]
    ret

HigherHalfStart:
    ;Finally into the higher half. No more dealing with that stupid crap with
    ;the virtual base and stuff. Let's get rid of that identity page

    mov dword [BootPageDirectory], 0 ;We need to specify DWORD so it clears the whole entry
    invlpg [0] ;And we invalidate that page



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
