#include "IO.hpp"
#include "E9TextDevice.hpp"


namespace Arch::I686::IO
{
	size_t E9TextDevice::Read (		 uint8_t* data, size_t size)
	{
		return 0;
	}
	size_t E9TextDevice::Write(const uint8_t* data, size_t size)
	{
		size_t i=0;
		while(data[i] && i < size)
		{
			Out(0xE9, data[i]);
			i++;
		}
		return i;
	}
}