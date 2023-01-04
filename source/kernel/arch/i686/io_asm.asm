global i8259_outb
i8259_outb:
	[bits 32]
	mov dx, [esp + 4]
	mov al, [esp + 8]
	out dx, al
	ret

global i8259_inb
i8259_inb:
	[bits 32]
	mov dx, [esp + 4]
	xor eax, eax
	in al, dx
	ret

global i8259_Panic
i8259_Panic:
	cli
	hlt

global i8259_EnabledInterrupts
i8259_EnabledInterrupts:
	sti
	ret

global i8259_DisabledInterrupts
i8259_DisabledInterrupts:
	cli
	ret

global crash_me
crash_me:
	;mov eax, 0
	;div eax
	;mov ecx, 0x1337
	;mov eax, 0
	;div eax
	int 0x80
	ret