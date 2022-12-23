#pragma once

#include <stdint.h>
#include <stdbool.h>

typedef struct 
{
	uint8_t  id;
	uint16_t cylinders;
	uint16_t sectors;
	uint16_t heads;

} Disk;

bool Disk_Init		 (Disk* disk, uint8_t driveNumber);
bool Disk_ReadSectors(Disk* disk, uint32_t lba, uint8_t sectors, void* lowerDataOut);
