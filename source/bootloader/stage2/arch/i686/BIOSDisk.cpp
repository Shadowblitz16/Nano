#include "Memory.hpp"
#include "BIOSDisk.hpp"
#include "BIOSDiskHelper.hpp"
#include <core/Defs.hpp>
#include <core/Assert.hpp>
#include <core/Memory.hpp>
#include <core/cpp/Algorithm.hpp>


BIOSDisk::BIOSDisk(uint8_t id)
	: 
	m_id(id), 
	m_pos(-1), 
	m_size(0)
{
	
}
bool   BIOSDisk::Init()
{
	m_haveExtensions = i686_Disk_HasExtensions(m_id);
	if (m_haveExtensions)
	{
		ExtendedDriveParams params;
		params.ParamsSize = sizeof(ExtendedDriveParams);
		if (!i686_Disk_ReadExtendedDriveParams(m_id, &params))
			return false;

		Assert(params.BytesPerSector == SectorSize);
		m_size = SectorSize * params.Sectors;
	}
	else
	{
		uint8_t driveType;
		if (!i686_Disk_ReadDefaultDriveParams(m_id, &driveType, &m_cylinders, &m_sectors, &m_heads))
			return false;

	}

	return true;
}

size_t BIOSDisk::Read (      uint8_t* data, size_t size)
{

	size_t initialPos = m_pos;
	if (m_pos == -1)
	{
		ReadNextSector();
		m_pos = 0;
	}

	if (m_pos >= m_size)
	{
		return 0;
	}

	while(size > 0)
	{
		size_t bufferPos  = m_pos % SectorSize;
		size_t bufferRead = Min(size, SectorSize - bufferPos);
		Memory::Copy(data, m_buffer + bufferPos, bufferRead);

		size  -= bufferRead;
		data  += bufferRead;
		m_pos += bufferRead;

		if (size > 0)
		{
			ReadNextSector();
		}
	}
	return m_pos - initialPos;
}

bool BIOSDisk::ReadNextSector()
{
	bool 	 ok  = false;
	uint64_t lba = m_pos / SectorSize;

	if (m_haveExtensions)
	{
		ExtendedDriveData data;
		data.ParamsSize 	= sizeof(ExtendedDriveParams);
		data.Reserved   	= 0;
		data.Count 		= 1;
		data.Buffer		= ToSegOffset(m_buffer);
		data.LBA 			= lba;

		for (int i = 0; i < 3; i++)
		{
			ok = i686_Disk_ReadExtendedDriveData(m_id, &data);
			if (!ok) i686_Disk_Reset(m_id);
		}
	}
	else
	{
		uint16_t cylinder, sector, head;
		LBA2CHS(lba, &cylinder, &sector, &head);

		for (int i=0; i < 3; i++)
		{
			ok = i686_Disk_ReadDefaultDriveData(m_id, cylinder, sector, head, 1, m_buffer);
			if (!ok) i686_Disk_Reset(m_id);
		}
	}

	return ok;
}
void BIOSDisk::LBA2CHS(uint32_t lba, uint16_t* cylinderOut, uint16_t* sectorOut, uint16_t* headOut)
{
	*sectorOut   =  lba % m_sectors + 1;
	*cylinderOut = (lba / m_sectors) / m_heads;
	*headOut     = (lba / m_sectors) % m_heads;
}

size_t BIOSDisk::Write(const uint8_t* data, size_t size)
{
	return 0;
}

void   BIOSDisk::Seek (SeekMode pos, int rel)
{
	switch (pos)
	{
		case SeekMode::Set:
			m_pos = -1;
			break;
		case SeekMode::Cur:
			m_pos += rel;
			ReadNextSector();
			break;
		case SeekMode::End:
			m_pos = m_size;
			break;
	}
}
size_t BIOSDisk::Size ()
{
	return m_size;
}

size_t BIOSDisk::Position()
{
	return m_pos;
}