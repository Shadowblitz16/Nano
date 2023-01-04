#include "io.h"

#define UNUSED_PORT 0x80

void i8259_iowait()
{
	i8259_outb(UNUSED_PORT, 0);
}