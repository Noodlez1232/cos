[GLOBAL load_gdt]		;Allows it to be called from C

load_gdt:
	mov eax, [esp+4]	;Load the GDT pointer that was passed as a parameter from C
	lgdt [eax]			;Load that pain in the ass

	mov ax, 0x10		;0x10 is the offset that the GDT is at
	mov ds, ax			;Load all the segment selectors
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax
	jmp 0x08:.flush
.flush:
   ret					;Get the hell out of dodge

[GLOBAL load_idt]		;Allows it to be called from C

[extern idtp]
load_idt:
    lidt [idtp]
    ret			;At least this one doesn't have anything else to do, so let's get the hell outa here.