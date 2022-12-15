#include "StdIo.h"
#include "StdInt.h"
#include "x86.h"

void PutChar  (char c)
{
	x86_Video_WriteCharTeletype(c, 0);
}
void PutString(const char* str, u32 count)
{
	while(*str && count > 0)
	{
		PutChar(*str);
		str++;
		count--;
	}
}
