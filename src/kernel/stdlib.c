#include "stdlib.h"
#include "stdint.h"

void qsort_internel(void* base, size_t num, size_t size, size_t left, size_t right, int(*comparer)(const void*, const void*))
{
	if (left >= right) 
		return;

	int i1 = left;
	int i2 = right;
	void* pivot = (base + (i1 * size));
	uint8_t tmp;
	for (;;)
	{
		while ((*comparer)(base + (i1 * size), pivot) < 0) i1++;
		while ((*comparer)(pivot, base + (i2 * size)) < 0) i2++;
		if (i1 >= i2) break;

		// swap
		for (int k =0; k < size; k++)
		{
			tmp = *((uint8_t*)(base + (i1 * size)) + k);
			*((uint8_t*)(base + (i1 * size)) + k) = *((uint8_t*)(base + (i2 * size)) + k);
			*((uint8_t*)(base + (i2 * size)) + k) = tmp;
		}
		i1++;
		i2--;
	}

	qsort_internel(base, num, size, left, i1 - 1, comparer);	
	qsort_internel(base, num, size, i2 - 1, right, comparer);	
}
void qsort		   (void* base, size_t num, size_t size, int(*comparer)(const void*, const void*))
{
	qsort_internel(base, num, size, 0, num - 1, comparer);
}
