[bits 32]

extern i8259_ISR_Handler;

%macro ISR_NOERRORCODE 1

global i8259_ISR%1
i8259_ISR%1:
	push 0 				; push dummy error code
	push %1				; push interrupt number
	jmp isr_common

%endmacro
%macro ISR_ERRORCODE 1
global i8259_ISR%1
i8259_ISR%1:
						; cpu pushes an error code to the stack
	push %1				; push interrupt number
	jmp isr_common
%endmacro

%include "arch/i686/isrs_gen.inc"

isr_common:
	pusha 				; pushes edi, esi, ebp, esp, ebx, edx, ecx, eax

	xor eax, eax		; push ds
	mov ax, ds
	push eax

	mov ax, 0x10		; use kernel data segment
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax

	push esp			; pass pointer to stack to C, so we can access all the pushed infomation
	call i8259_ISR_Handler
	add esp, 4

	pop eax				; restore old segment
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax

	popa				; pop what we pushed with pusha
	add esp, 8			; remove error code and interrupt number
	iret				; will pop: cs, eip, eflags, ss, esp
