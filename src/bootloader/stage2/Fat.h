#pragma once

#include "Disk.h"
#include "StdInt.h"
#include "StdBool.h"

#pragma pack(push, 1)
typedef struct 
{
	u8 Name[11];
    u8 Attributes;
	u8 Reserved;
	u8 CreatedTimeTenths;
	u16 CreatedTime;
	u16 CreatedDate;
	u16 AccessedDate;
	u16 FirstClusterHigh;
	u16 ModifedTime;
	u16 ModifedDate;
	u16 FirstClusterLow;
	u32 Size;
} FatDirectoryEntry;
#pragma pack(pop)


typedef struct
{
	int 	Handle;
	bool 	IsDirectory;
	u32 	Position;
	u32 	Size;

} FatFile;

enum FatAttributes
{
	FAT_ATTRIBUTE_READONLY 	= 0x01,
	FAT_ATTRIBUTE_HIDDEN   	= 0x02,
	FAT_ATTRIBUTE_SYSTEM    = 0x04,
	FAT_ATTRIBUTE_VOLUME_ID = 0x08,
	FAT_ATTRIBUTE_DIRECTORY = 0x10,
	FAT_ATTRIBUTE_ARCHIVE   = 0x20,
	FAT_ATTRIBUTE_LFN    	= FAT_ATTRIBUTE_READONLY | FAT_ATTRIBUTE_HIDDEN | FAT_ATTRIBUTE_SYSTEM | FAT_ATTRIBUTE_VOLUME_ID
};

bool 	 	 FatInit(Disk* disk);
FatFile far* FatOpen(Disk* disk, const char* path);
u32  	 	 FatRead(Disk* disk, FatFile far* file, u32 byteCount, void* dataOut);
bool 	 	 FatReadEntry(Disk* disk, FatFile far* file, FatDirectoryEntry* dirEntry);
void 	 	 FatClose(FatFile far* file);
