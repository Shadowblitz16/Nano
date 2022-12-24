#include "disk.h"
#include "x86.h"

bool Disk_Init(Disk* disk, uint8_t driveNumber)
{
	uint8_t  driveType;
	uint16_t cylinders, sectors, heads;

	disk->id = driveNumber;

	if (!x86_Disk_GetDriveParams(disk->id, &driveType, &cylinders, &sectors, &heads))
		return false;

	disk->id 		= driveNumber;
	disk->cylinders = cylinders;
	disk->heads     = heads;
    disk->sectors   = sectors;

	return true;
}
void Disk_LBA2CHS(Disk* disk, uint32_t lba, uint16_t* cylindersOut, uint16_t* sectorsOut, uint16_t* headsOut)
{
	// sectors = (LBA % sectors per track + 1)
	*sectorsOut = lba % disk->sectors + 1;

	// cylinders = (LBA % sectors per track) / heads
	*cylindersOut = (lba / disk->sectors) / disk->heads;

	// heads = (LBA / sectors per track) % heads
	*headsOut = (lba / disk->sectors) % disk->heads;

}

bool Disk_ReadSectors(Disk* disk, uint32_t lba, uint8_t sectors, void* lowerDataOut)
{
	uint16_t cylinder, sector, head;
	Disk_LBA2CHS(disk, lba, &cylinder, &sector, &head);

	for (int i=0; i<3; i++)
	{
		if	(x86_Disk_Read(disk->id, cylinder, sector, head, sectors, lowerDataOut))
			return true;

		x86_Disk_Reset(disk->id);
	}
	return false;
}
