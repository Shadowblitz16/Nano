#pragma once
#include <core/dev/BlockDevice.hpp>

class BIOSDisk : public BlockDevice
{
	public:
	
	BIOSDisk(uint8_t id);

	bool 		   Init();
	virtual size_t Read (      uint8_t* data, size_t size) override;
	virtual size_t Write(const uint8_t* data, size_t size) override;

	virtual void   Seek     (SeekMode pos, int rel) override;
	virtual size_t Size     () override;
	virtual size_t Position () override;

	private:

	bool ReadNextSector();
	void LBA2CHS(uint32_t lba, uint16_t* cylinderOut, uint16_t* sectorOut, uint16_t* headOut);

	static inline const constexpr int SectorSize = 512;

	uint8_t  m_id;
	bool     m_haveExtensions;
	uint16_t m_cylinders;
	uint16_t m_sectors;
	uint16_t m_heads;

	uint8_t  m_buffer[SectorSize];
	uint64_t m_pos;
	uint64_t m_size;	
};