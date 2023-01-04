#pragma once
#include <stdbool.h>
#include <stdint.h>

void 	__attribute__((cdecl)) i8259_outb(uint16_t port, uint8_t value);
uint8_t __attribute__((cdecl)) i8259_inb (uint16_t port);
uint8_t __attribute__((cdecl)) i8259_EnabledInterrupts ();
uint8_t __attribute__((cdecl)) i8259_DisabledInterrupts ();

void i8259_iowait();
__attribute__((cdecl)) void i8259_Panic();