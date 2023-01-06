#pragma once
#include <dev/CharacterDevice.hpp>

namespace Arch::I686 
{
	class E9Device : public CharacterDevice
	{
		public:
			virtual size_t Read(uint8_t* buffer, size_t size);
			virtual size_t Write(const uint8_t* buffer, size_t size);
	};
}