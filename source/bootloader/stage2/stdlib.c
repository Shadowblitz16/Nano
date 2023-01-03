#include "stdlib.h"
#include "stdint.h"

void qsort_internal(void* base, size_t num, size_t size, size_t left, size_t right, int(*comparer)(const void*, const void*))
{
    if (left >= right)
        return;

    int i = left, j = right;
    void* pivot = base + (i * size);
    uint8_t temp;

    for (;;) 
    {
        while ((*comparer)(base + (i * size), pivot) < 0) i++;
        while ((*comparer)(pivot, base + (j * size)) < 0) j--;
        if (i >= j)
            break;

        // swap
        for (int k = 0; k < size; k++)
        {
            temp = *((uint8_t*)(base + (i * size)) + k);
            *((uint8_t*)(base + (i * size)) + k) = *((uint8_t*)(base + (j * size)) + k);
            *((uint8_t*)(base + (j * size)) + k) = temp;
        }
        i++;
        j--;
    }

    qsort_internal(base, num, size, left, i - 1, comparer);
    qsort_internal(base, num, size, j + 1, right, comparer);
}
void qsort		   (void* base, size_t num, size_t size, int(*comparer)(const void*, const void*))
{
	qsort_internal(base, num, size, 0, num - 1, comparer);
}
