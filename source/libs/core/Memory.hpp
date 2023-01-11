#pragma once
#include <stddef.h>
#include <core/Defs.hpp>

EXPORT void ASMCALL memcpy(void* destination, const void* source, size_t count);

namespace Memory
{
	constexpr auto Copy = memcpy;
}