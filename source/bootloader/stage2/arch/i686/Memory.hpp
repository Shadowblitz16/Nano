#pragma once
#include <stdint.h>

template<typename T>
uint32_t ToSegOffset(T address)
{
	uint32_t addr = reinterpret_cast<uint32_t>(address);
	uint32_t seg  = (addr >> 4) & 0xFFFF;
	uint32_t off  = (addr >> 0) & 0xF;

	return (seg << 16) | off;
}

template<typename T>
T ToLinear(uint32_t segOffset)
{
	uint32_t off  = (uint32_t)(segOffset) & 0xFFFF;
	uint32_t seg  = (uint32_t)(segOffset) >> 16;
	uint32_t addr = seg * 16 + off;
	return reinterpret_cast<T>(addr);
}
