#pragma once

#include "StdInt.h"

void far* 	MemoryCopy(void far* destination, const void far* source, u16 num);
void far* 	MemorySet (void far* destination, int value, u16 num);
int 		MemoryCompare(const void far* ptr1, const void far* ptr2, u16 num);
