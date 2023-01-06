#include "hal.h"
#include <arch/i686/gdt.h>
#include <arch/i686/idt.h>
#include <arch/i686/isr.h>
#include <arch/i686/irq.h>
#include <arch/i686/vga_text.h>

void HAL_Init()
{
	VGA_Clear();

	i8259_GDT_Init();
	i8259_IDT_Init();
	i8259_ISR_Init();
	i8259_IRQ_Init();
}