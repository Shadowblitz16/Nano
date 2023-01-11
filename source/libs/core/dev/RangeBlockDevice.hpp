#pragma once

#include "BlockDevice.hpp"

class RangeBlockDevice : public BlockDevice
{
	public:
	RangeBlockDevice();
	void Init(BlockDevice* device, size_t begin, size_t size);
	virtual size_t Read (      uint8_t* data, size_t size) override;
	virtual size_t Write(const uint8_t* data, size_t size) override;
	virtual void   Seek (SeekMode mode, int rel) override;
	virtual size_t Size () override;
	virtual size_t Position () override;

	private:
	BlockDevice* m_device;
	size_t m_begin;
	size_t m_size;
};