#pragma once
#include <stddef.h>
#include <stdint.h>
#include "CharacterDevice.hpp"

enum class SeekMode
{
	Set,
	Cur,
	End
};

class BlockDevice : public CharacterDevice
{
	public:
	virtual void   Seek    (SeekMode mode, int rel) = 0;
	virtual size_t Size    () = 0;
	virtual size_t Position() = 0;
};