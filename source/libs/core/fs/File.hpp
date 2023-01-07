#pragma once
#include <stddef.h>
#include <stdint.h>
#include <core/dev/BlockDevice.hpp>


class File : public BlockDevice
{
	public:
	virtual ~File() { }

	virtual void   Seek (SeekMode pos, int rel) = 0;

	virtual size_t Read (      uint8_t* data, size_t size) = 0;
	virtual size_t Write(const uint8_t* data, size_t size) = 0;
	virtual size_t Size () = 0;
};