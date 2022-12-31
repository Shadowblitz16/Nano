#include "mbr.h"
#include "memory.h"
#include "stdio.h"

typedef struct 
{
	uint8_t attributes;
	uint8_t chs[3];
	uint8_t partitionType;
	uint8_t chsEnd[3];
	uint32_t lbaStart;
	uint32_t size;
} __attribute__((packed)) MBR_Entry;


void MBR_DetectPartition(Partition* partition, Disk* disk, void* partitionEntry)
{
	partition->Disk = disk;
	if (disk->id < 0x80) 
	{
		partition->Offset = 0;
		partition->Size 	 = ((uint32_t)
		(
			(uint32_t)(disk->cylinders) * 
			(uint32_t)(disk->heads    ) * 
			(uint32_t)(disk->sectors  )
		));
	}
	else
	{
		MBR_Entry* entry  = (MBR_Entry*)segoffset_to_linear(partitionEntry);
		partition->Offset = entry->lbaStart;
		partition->Size   = entry->size;
	}
}

bool	 MBR_ReadSectors(Partition* partition, uint32_t lba, uint8_t sectors, void* lowerDataOut)
{
	return Disk_ReadSectors(partition->Disk, lba + partition->Offset, sectors, lowerDataOut);
}