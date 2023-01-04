#pragma once
#include <stdint.h>

typedef struct {
	uint32_t ds;											// data segment
	uint32_t edi, esi, ebp, kern_esp, ebx, edx, ecx, eax;	// pusha
	uint32_t interrupt, error;								// we push interrupt, error is pushed
	uint32_t eip, cs, eflags, esp, ss;						// pushed automaticly by CPU
} __attribute__((packed)) Registers;

typedef void (*ISRHandler)(Registers* regs);

void i8259_ISR_Init();
void i8259_ISR_RegisterHandler(int interrupt, ISRHandler handler);