#include "IO.hpp"
#include "E9Device.hpp"


namespace Arch::I686 
{
	size_t E9Device::Read (		 uint8_t* data, size_t size)
	{
		return 0;
	}
	size_t E9Device::Write(const uint8_t* data, size_t size)
	{
		for (size_t i = 0; i < size; i++)
			Out(0xE9, data[i]);
		
		return size;
	}
}