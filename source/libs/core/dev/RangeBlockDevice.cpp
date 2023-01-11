#include "RangeBlockDevice.hpp"
#include <core/cpp/Algorithm.hpp>

RangeBlockDevice::RangeBlockDevice()
: m_device(nullptr),
  m_begin(0),
  m_size(0),
{

}
void RangeBlockDevice::Init(BlockDevice* device, size_t begin, size_t size)
{
	m_device = device;
	m_begin  = begin;
	m_size   = size;
	m_device->Seek(SeekMode::Set, m_begin);
}

size_t RangeBlockDevice::Read (      uint8_t* data, size_t size)
{
	if (m_device == nullptr) return 0;
	size = Min(size, Size() - Position());
	return m_device->Read(data, size);
}
size_t RangeBlockDevice::Write(const uint8_t* data, size_t size)
{
	if (m_device == nullptr) return 0;
	size = Min(size, Size() - Position());
	return m_device->Write(data, size);
}
void   RangeBlockDevice::Seek (SeekMode mode, int rel)
{
	if (m_device == nullptr) return;
	switch (mode)
	{
		case SeekMode::Set:
			m_device->Seek(SeekMode::Set, m_begin + rel);
			break;
		case SeekMode::Cur:
			m_device->Seek(SeekMode::Cur, rel);
			break;
		case SeekMode::End:
			m_device->Seek(SeekMode::Set, m_begin + m_size);
			break;
	}
}

size_t RangeBlockDevice::Size     () 
{
	return m_size;
}
size_t RangeBlockDevice::Position () 
{
	return m_device->Position() - m_begin;
}
