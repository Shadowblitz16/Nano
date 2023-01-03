global x86_outb
x86_outb:
	[bits 32]
	mov dx, [esp + 4]
	mov al, [esp + 8]
	out dx, al
	ret

global x86_inb
x86_inb:
	[bits 32]
	mov dx, [esp + 4]
	xor eax, eax
	in al, dx
	ret

global i686_Panic
i686_Panic:
	cli
	hlt

global crash_me
crash_me:
	;mov eax, 0
	;div eax
	mov ecx, 0x1337
	mov eax, 0
	div eax
	ret