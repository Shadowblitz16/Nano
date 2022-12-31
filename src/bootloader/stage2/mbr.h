#pragma once
#include "disk.h"
#include <stdbool.h>
#include <stdint.h>

typedef struct
{
	Disk* 		Disk;
	uint32_t 	Offset;
	uint32_t    Size;
} Partition;

void 	MBR_DetectPartition(Partition *partition, Disk* disk, void* partitionEntry);
bool	MBR_ReadSectors	(Partition* partition, uint32_t lba, uint8_t sectors, void* lowerDataOut);
