#pragma once

#include "StdInt.h"
#include "StdBool.h"

typedef struct 
{
	u8  id;
	u16 cylinders;
	u16 sectors;
	u16 heads;

} Disk;

bool DiskInit(Disk* disk, u8 driveNumber);
bool DiskReadSectors(Disk* disk, u32 lba, u8 sectors, void far* dataOut);
