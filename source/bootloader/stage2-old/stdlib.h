#pragma once
#include <stddef.h>

typedef int(*Comparer)(const void*, const void*);
void qsort(void* base, size_t num, size_t size, Comparer comparer);