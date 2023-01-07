#include "memory.h"

void* memcpy(void* destination, const void* source, uint16_t num)
{
	uint8_t* u8Destination = (uint8_t*)destination;
	const uint8_t* u8Source = (const uint8_t*)source;

	for (uint16_t i=0; i<num; i++)
		u8Destination[i] = u8Source[i];

	return destination;
}
void* memset(void* ptr, int value, uint16_t num)
{
	uint8_t* u8Ptr = (uint8_t*)ptr;

	for (uint16_t i=0; i<num; i++)
	    u8Ptr[i] = (uint8_t)value;

    return ptr;
}
int   memcmp(const void* ptr1, const void* ptr2, uint16_t num)
{
	const uint8_t* u8Ptr1 = (const uint8_t*)ptr1;
	const uint8_t* u8Ptr2 = (const uint8_t*)ptr2;

	for (uint16_t i=0; i<num; i++)
		if (u8Ptr1[i] != u8Ptr2[i])
			return 1;
	
	return 0;
}

void*   segoffset_to_linear(void* addr)
{
	uint32_t offset  = (uint32_t)(addr) & 0xFFFF;
	uint32_t segment = (uint32_t)(addr) >> 16;
	return (void*)(segment * 16 + offset);
}
