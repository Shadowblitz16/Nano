#pragma once

#include <stdint.h>

void* 	memcpy(void* destination, const void* source, uint16_t num);
void* 	memset(void* destination, int value, uint16_t num);
int 	memcmp(const void* ptr1, const void* ptr2, uint16_t num);
