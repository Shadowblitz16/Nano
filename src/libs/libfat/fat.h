#pragma once

#include "disk.h"
#include <stdint.h>
#include <stdbool.h>

typedef struct 
{
	uint8_t  Name[11];
    uint8_t  Attributes;
	uint8_t  Reserved;
	uint8_t  CreatedTimeTenths;
	uint16_t CreatedTime;
	uint16_t CreatedDate;
	uint16_t AccessedDate;
	uint16_t FirstClusterHigh;
	uint16_t ModifedTime;
	uint16_t ModifedDate;
	uint16_t FirstClusterLow;
	uint32_t Size;
} __attribute__((packed)) Fat_DirectoryEntry;


typedef struct
{
	int 	Handle;
	bool 	IsDirectory;
	uint32_t 	Position;
	uint32_t 	Size;

} Fat_File;

enum Fat_Attributes
{
	FAT_ATTRIBUTE_READONLY 	= 0x01,
	FAT_ATTRIBUTE_HIDDEN   	= 0x02,
	FAT_ATTRIBUTE_SYSTEM    = 0x04,
	FAT_ATTRIBUTE_VOLUME_ID = 0x08,
	FAT_ATTRIBUTE_DIRECTORY = 0x10,
	FAT_ATTRIBUTE_ARCHIVE   = 0x20,
	FAT_ATTRIBUTE_LFN    	= FAT_ATTRIBUTE_READONLY | FAT_ATTRIBUTE_HIDDEN | FAT_ATTRIBUTE_SYSTEM | FAT_ATTRIBUTE_VOLUME_ID
};

bool 	 	Fat_Init(Disk* disk);
Fat_File* 	Fat_Open(Disk* disk, const char* path);
uint32_t   	Fat_Read(Disk* disk, Fat_File* file, uint32_t byteCount, void* dataOut);
bool 	 	Fat_ReadEntry(Disk* disk, Fat_File* file, Fat_DirectoryEntry* dirEntry);
void 	 	Fat_Close(Fat_File* file);
