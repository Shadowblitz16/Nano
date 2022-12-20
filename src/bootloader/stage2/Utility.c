#include "Utility.h"

u32 Align(u32 number, u32 alignTo)
{
	if (alignTo == 0) return number;

	u32 rem = number % alignTo;
	return (rem > 0) ? (number + alignTo - rem) : number;
}
