#include "Disk.h"
#include "x86.h"

bool DiskInit(Disk* disk, u8 driveNumber)
{
	u8  driveType;
	u16 cylinders, sectors, heads;

	disk->id = driveNumber;

	if (!x86DiskGetDriveParams(disk->id, &driveType, &cylinders, &sectors, &heads))
		return false;

	disk->id 		= driveNumber;
	disk->cylinders = cylinders + 1;
	disk->heads     = heads 	+ 1;
    disk->sectors   = sectors;

	return true;
}
void DiskLBA2CHS(Disk* disk, u32 lba, u16* cylindersOut, u16* sectorsOut, u16* headsOut)
{
	// sectors = (LBA % sectors per track + 1)
	*sectorsOut = lba % disk->sectors + 1;

	// cylinders = (LBA % sectors per track) / heads
	*cylindersOut = (lba / disk->sectors) / disk->heads;

	// heads = (LBA / sectors per track) % heads
	*headsOut = (lba / disk->sectors) % disk->heads;

}

bool DiskReadSectors(Disk* disk, u32 lba, u8 sectors, void far* dataOut)
{
	u16 cylinder, sector, head;
	DiskLBA2CHS(disk, lba, &cylinder, &sector, &head);

	for (int i=0; i<3; i++)
	{
		if	(x86DiskRead(disk->id, cylinder, sector, head, sectors, dataOut))
			return true;

		x86DiskReset(disk->id);
	}
	return false;
}
