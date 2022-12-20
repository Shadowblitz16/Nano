#include "Memory.h"

void far* MemoryCopy(void far* destination, const void far* source, u16 num)
{
	u8 far* u8Destination = (u8 far*)destination;
	const u8 far* u8Source = (const u8 far*)source;

	for (u8 i=0; i<num; i++)
		u8Destination[i] = u8Source[i];

	return destination;
}

void far* 	MemorySet (void far* ptr, int value, u16 num)
{
	u8 far* u8Ptr = (u8 far*)ptr;

	for (u8 i=0; i<num; i++)
	    u8Ptr[i] = (u8)value;

    return ptr;
}

int 		MemoryCompare(const void far* ptr1, const void far* ptr2, u16 num)
{
	const u8 far* u8Ptr1 = (const u8 far*)ptr1;
	const u8 far* u8Ptr2 = (const u8 far*)ptr2;

	for (u16 i=0; i<num; i++)
		if (u8Ptr1[i] != u8Ptr2[i])
			return 1;
	
	return 0;
}
