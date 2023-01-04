#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

typedef int (*Comparer)(const void*, const void*);

void qswap_internal(uint8_t* a, uint8_t* b)
{
    uint8_t tmpA = *a;
	uint8_t tmpB = *b;
    *a = tmpB;
    *b = tmpA;
}

void qsort_internal(void* base, size_t num, size_t size, Comparer compar, size_t left, size_t right)
{
    if (left >= right)
        return;

    size_t 	l	 	= left; 
	size_t 	r 		= right;
    void* 	pivot 	= base + (l * size);

    for (;;) 
    {
        while ((*compar)(base + (l * size), pivot) < 0) l++;
        while ((*compar)(pivot, base + (r * size)) < 0) r--;
        if (l >= r)
			break;

        // Swap
        for (int k = 0; k < size; k++)
        {
			uint8_t tempA = *((uint8_t*)(base + (l * size)) + k);
			uint8_t tempB = *((uint8_t*)(base + (r * size)) + k);
			
			qswap_internal(&tempA, &tempB);
			
			*((uint8_t*)(base + (l * size)) + k) = tempA;
			*((uint8_t*)(base + (r * size)) + k) = tempB;

        }

        l++;
        r--;
    }

    qsort_internal(base, num, size, compar, left, l - 1 );
    qsort_internal(base, num, size, compar, r + 1, right);
}

void qsort(void* base, size_t num, size_t size, Comparer compar)
{
	qsort_internal(base, num, size, compar, 0, num - 1);
}
