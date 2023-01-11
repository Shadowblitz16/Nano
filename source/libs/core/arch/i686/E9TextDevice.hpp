#pragma once
#include <stddef.h>
#include <core/dev/CharacterDevice.hpp>

namespace Arch::I686::IO
{
	class E9TextDevice : public CharacterDevice
	{
		public:
			virtual size_t Read(uint8_t* buffer, size_t size);
			virtual size_t Write(const uint8_t* buffer, size_t size);
	};
}