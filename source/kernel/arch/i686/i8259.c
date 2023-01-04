#include "pic.h"
#include "io.h"
#include <stdbool.h>

#define PIC1_COMMAND_PORT 	0x20
#define PIC1_DATA_PORT 		0x21
#define PIC2_COMMAND_PORT 	0xA0
#define PIC2_DATA_PORT 		0xA1

typedef enum {
	PIC_ICW1_ICW4		= 0x01,
	PIC_ICW1_SINGLE		= 0x02,
	PIC_ICW1_INTERVAL4	= 0x04,
	PIC_ICW1_LEVEL		= 0x08,
	PIC_ICW1_INIT		= 0x10,
} PIC_ICW1;

typedef enum {
	PIC_ICW4_8086			= 0x1,
	PIC_ICW4_AUTO_EOI		= 0x2,
	PIC_ICW4_BUFFER_MASTER	= 0x4,
	PIC_ICW4_BUFFER_SLAVE	= 0x0,
	PIC_ICW4_BUFFERED		= 0x8,
	PIC_ICW4_SFNM			= 0x10
} PIC_ICW4;


typedef enum {
	PIC_CMD_END_OF_INTERRUPT	= 0x20,
	PIC_CMD_READ_IRR			= 0x0A,
	PIC_CMD_READ_ISR			= 0x0B
} PIC_CMD;

static uint16_t g_PicMask = 0xFFFF;
static bool     g_AutoEoi = false;

void i8259_PIC_SetMask(uint16_t newMask)
{
	g_PicMask = newMask;
	i8259_outb(PIC1_DATA_PORT, newMask & 0xFF);
	i8259_iowait();
	i8259_outb(PIC2_DATA_PORT, newMask >> 8);
	i8259_iowait();
}

uint16_t i8259_PIC_GetMask()
{
	return i8259_inb(PIC1_DATA_PORT) | (i8259_inb(PIC2_DATA_PORT) << 8);
}

void i8259_PIC_Configure(uint8_t offsetPic1, uint8_t offsetPic2, bool autoEoi)
{
	// mask everything
	i8259_PIC_SetMask(0xFFFF);

	// initialize control word 1
	i8259_outb(PIC1_COMMAND_PORT, PIC_ICW1_ICW4 | PIC_ICW1_INIT);
	i8259_iowait();
	i8259_outb(PIC2_COMMAND_PORT, PIC_ICW1_ICW4 | PIC_ICW1_INIT);
	i8259_iowait();

	// initialize control word 2 - the offsets
	i8259_outb(PIC1_DATA_PORT, offsetPic1);
	i8259_iowait();
	i8259_outb(PIC2_DATA_PORT, offsetPic2);
	i8259_iowait();

	// initialize control word 3
	i8259_outb(PIC1_DATA_PORT, 0x4);		// tell PIC1 that it has a slave at IQR2 (0000 0100)
	i8259_iowait();
	i8259_outb(PIC2_DATA_PORT, 0x2);		// tell PIC2 that its cascade identity (0000 0100)
	i8259_iowait();	

	// initialize control word 4
	uint8_t icw4 = PIC_ICW4_8086;
	if (autoEoi) {
		icw4 |= PIC_ICW4_AUTO_EOI;
	}

	i8259_outb(PIC1_DATA_PORT, autoEoi);
	i8259_iowait();
	i8259_outb(PIC2_DATA_PORT, autoEoi);
	i8259_iowait();	

	// mask all interrupts until they are nebaled by the device drive
	i8259_PIC_SetMask(0xFFFF);
}

void i8259_PIC_SendEndOfInterrupt(int irq)
{
	if (irq >= 8)
		i8259_outb(PIC2_COMMAND_PORT, PIC_CMD_END_OF_INTERRUPT);
	i8259_outb(PIC1_COMMAND_PORT, PIC_CMD_END_OF_INTERRUPT);
}

void i8259_PIC_Disable()
{
	// clear data registers
	i8259_PIC_SetMask(0xFFFF);
}

void i8259_PIC_Mask(int irq)
{
	i8259_PIC_SetMask(g_PicMask |  (1 << irq));
}

void i8259_PIC_Unmask(int irq)
{
	i8259_PIC_SetMask(g_PicMask & ~(1 << irq));
}

uint16_t i8259_PIC_ReadIrqRequestRegister()
{
	i8259_outb(PIC1_COMMAND_PORT, PIC_CMD_READ_IRR);
	i8259_outb(PIC2_COMMAND_PORT, PIC_CMD_READ_IRR);
	return (i8259_inb(PIC2_COMMAND_PORT) | (i8259_inb(PIC2_COMMAND_PORT) << 8));
}

uint16_t i8259_PIC_ReadInServiceRegister()
{
	i8259_outb(PIC1_COMMAND_PORT, PIC_CMD_READ_ISR);
	i8259_outb(PIC2_COMMAND_PORT, PIC_CMD_READ_ISR);
	return (i8259_inb(PIC2_COMMAND_PORT) | (i8259_inb(PIC2_COMMAND_PORT) << 8));
}

bool i8259_PIC_Probe()
{
	i8259_PIC_Disable();
	i8259_PIC_SetMask(0x1337);
	return i8259_PIC_GetMask() == 0x1337;
}

static const PICDriver g_PicDriver = {
	.Name 				= "8259 PIC",
	.Init 				= &i8259_PIC_Configure,
	.Probe 				= &i8259_PIC_Probe,
	.Disable 			= &i8259_PIC_Disable,
	.Mask 				= &i8259_PIC_Mask,
	.Unmask 			= &i8259_PIC_Unmask,
	.SendEndOfInterrupt = &i8259_PIC_SendEndOfInterrupt,
};
const PICDriver* i8259_GetDriver()
{
	return &g_PicDriver;
}