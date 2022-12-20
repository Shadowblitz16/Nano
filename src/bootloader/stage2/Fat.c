#include "Fat.h"
#include "StdIO.h"
#include "Utility.h"
#include "MemDefs.h"
#include "String.h"
#include "Memory.h"
#include "CType.h"

#define SECTOR_SIZE 			512
#define MAX_PATH_SIZE        	256
#define MAX_FILE_HANDLES		10
#define ROOT_DIRECTORY_HANDLE	-1

#pragma pack(push, 1)
typedef struct
{
	u8 	BootJumpInstruction[3];
	u8 	OEMIdentifier[8];
	u16	BytesPerSector;
	u8	SectorsPerCluster;
	u16 ReservedSectors;
	u8 	FatCount;
	u16	DirEntryCount;
	u16	TotalSectors;
	u8	MediaDescriptorType;    
	u16	SectorsPerFat;
	u16 SectorsPerTrack;
	u16 Heads;
	u32 HiddenSectors;
    u32 LargeSectorCount;

	// extended boot record
	u8  DriveNumber;
	u8	Reserved;
	u8 	Signature;
	u32	VolumnID;
	u8  VolumnLabel[11];
	u8	SystemID[8];

	// We don't care about code.

} FatBootSector;
#pragma pack(pop)

typedef struct
{
	u8 		Buffer[SECTOR_SIZE];
	FatFile Public;
	bool 	Opened;
	u32     FirstCluster;
	u32 	CurrentCluster;
	u32 	CurrentSectorInCluster;
} FatFileData;

typedef struct 
{
	union
	{
		FatBootSector 	BootSector;
		u8		 		BootSectorBytes[SECTOR_SIZE];
	} BS;
	
	FatFileData Root;
	FatFileData	OpenedFiles[MAX_FILE_HANDLES];

} FatData;
static FatData 			far* 	data			= null;
static u8 				far*	fat 			= null;
static u32 						dataSectionLBA 	= 0;


bool FatReadBootSector(Disk* disk)
{
	return DiskReadSectors(disk, 0, 1, data->BS.BootSectorBytes);
}

bool FatReadFat(Disk* disk)
{
	return DiskReadSectors(disk, data->BS.BootSector.ReservedSectors, data->BS.BootSector.SectorsPerFat, fat);
}

bool FatInit(Disk* disk)
{
	data = (FatData far*)MEMORY_FAT_ADDR;

	// read boot sector
	if(!FatReadBootSector(disk))
	{
		PrintFormat("FAT: read boot sector failed!\r\n");
		return false;
	}

	// read FAT
	fat = (u8 far*)data + sizeof(FatData);
	u32 fatSize = data->BS.BootSector.BytesPerSector * data->BS.BootSector.SectorsPerFat;
	if (sizeof(FatData) + fatSize >= MEMORY_FAT_SIZE)
	{
		PrintFormat("FAT: not enough memory to read FAT! Requires %lu, only have %u\r\n", sizeof(FatData) + fatSize, MEMORY_FAT_SIZE);
		return false;
	}

	if (!FatReadFat(disk))
	{
		PrintFormat("FAT: read fat failed!\r\n");
		return false;	
	}

	// read root directory
	u32 rootDirLBA	= data->BS.BootSector.ReservedSectors + data->BS.BootSector.SectorsPerFat * data->BS.BootSector.FatCount;
	u32 rootDirSize = sizeof(FatDirectoryEntry) * data->BS.BootSector.DirEntryCount;

	// open opened root directory file
	data->Root.Public.Handle 			= ROOT_DIRECTORY_HANDLE;
	data->Root.Public.IsDirectory		= true;
	data->Root.Public.Position			= 0;
	data->Root.Public.Size 				= sizeof(FatDirectoryEntry) * data->BS.BootSector.DirEntryCount;
	data->Root.Opened 					= true;
	data->Root.FirstCluster				= rootDirLBA;
	data->Root.CurrentCluster			= rootDirLBA;
	data->Root.CurrentSectorInCluster	= 0;

	if (!DiskReadSectors(disk, rootDirLBA, 1, data->Root.Buffer))
	{
		PrintFormat("FAT: read root directory failed!\r\n");
		return false;	
	}

	// calulate data section
	u32 rootDirSectors = (rootDirSize + data->BS.BootSector.BytesPerSector - 1) / data->BS.BootSector.BytesPerSector;
	dataSectionLBA =	rootDirLBA + rootDirSectors;

	// reset opened files
	for (int i=0; i<MAX_FILE_HANDLES; i++)
		data->OpenedFiles[i].Opened = false;

	return true;
}

u32  FatClusterToLBA(u32 cluster)
{
	return dataSectionLBA + (cluster - 2) * data->BS.BootSector.SectorsPerCluster;
}

FatFile far* FatOpenEntry(Disk* disk, FatDirectoryEntry* entry)
{
	// find empty handle
	int handle = -1;
	for (int i=0; i<MAX_FILE_HANDLES && handle < 0; i++)
	{
		if (!data->OpenedFiles[i].Opened)
			handle = i;
	}

	// out of handles
	if (handle < 0)
	{
		PrintFormat("FAT: out of file handles!\r\n");
		return false;	
	}

	// setup vars
	FatFileData far* fd 		= &data->OpenedFiles[handle];
	fd->Public.Handle 			= handle;
	fd->Public.IsDirectory 		= (entry->Attributes & FAT_ATTRIBUTE_DIRECTORY) != 0;
	fd->Public.Position     	= 0;
	fd->Public.Size         	= entry->Size;
	fd->FirstCluster			= entry->FirstClusterLow + ((u32)entry->FirstClusterHigh << 16);
	fd->CurrentCluster      	= fd->FirstCluster;
	fd->CurrentSectorInCluster 	= 0;

	if (!DiskReadSectors(disk, FatClusterToLBA(fd->CurrentCluster), 1, fd->Buffer))
	{
		PrintFormat("FAT: read error!\r\n");
		return false;	
	}

	fd->Opened = true;
	return &fd->Public;
}

u32 FatNextCluster(u32 currentCluster)
{
	u32 fatIndex = currentCluster * 3 / 2;
	if (currentCluster % 2 == 0) return currentCluster = (*(u16 far*)(fat + fatIndex)) & 0x0FFF;
	else 					 	 return currentCluster = (*(u16 far*)(fat + fatIndex)) >> 4;
}

u32  	 	 FatRead(Disk* disk, FatFile far* file, u32 byteCount, void* dataOut)
{
	// get file data
	FatFileData far* fd = (file->Handle == ROOT_DIRECTORY_HANDLE) ? &data->Root : &data->OpenedFiles[file->Handle];

	u8*	u8DataOut = (u8*)dataOut; 

	// don't read past the end of file
	if (!fd->Public.IsDirectory)
		byteCount = Min(byteCount, fd->Public.Size - fd->Public.Position);

	while(byteCount > 0)
	{
		u32 left = SECTOR_SIZE - (fd->Public.Position % SECTOR_SIZE);
		u32 take = Min(byteCount, left);

		MemoryCopy(u8DataOut, fd->Buffer + fd->Public.Position % SECTOR_SIZE, take);
		fd->Public.Position += take;
		u8DataOut 			+= take;
		byteCount 			-= take;

		if (left == take)
		{
			// Special handling for root directory
			if (fd->Public.Handle == ROOT_DIRECTORY_HANDLE)
			{
				++fd->CurrentCluster;

				// read next sector
				if (!DiskReadSectors(disk, fd->CurrentCluster, 1, fd->Buffer))
				{
					PrintFormat("FAT: read error!\r\n");
					break;
				}
			}
			else
			{
				// calculate next cluster & sectors to read
				if (++fd->CurrentSectorInCluster >= data->BS.BootSector.SectorsPerCluster)
				{
					// read next cluster
					fd->CurrentSectorInCluster 	= 0;
					fd->CurrentCluster 			= FatNextCluster(fd->CurrentCluster);
				}

				if (fd->CurrentCluster >= 0xFF8)
				{
					fd->Public.Size = fd->Public.Position;
					break;
				}

				// read next sector
				if (!DiskReadSectors(disk, FatClusterToLBA(fd->CurrentCluster) + fd->CurrentSectorInCluster, 1, fd->Buffer))
				{
					PrintFormat("FAT: read error!\r\n");
					break;
				}
			}
		}
	}

	return u8DataOut - (u8*)dataOut;
}

bool 	 	 FatReadEntry(Disk* disk, FatFile far* file, FatDirectoryEntry* dirEntry)
{
	return FatRead(disk, file, sizeof(FatDirectoryEntry), dirEntry) == sizeof(FatDirectoryEntry);
}

void 	 	 FatClose(FatFile far* file)
{
	if (file->Handle == ROOT_DIRECTORY_HANDLE)
    {
		file->Position = 0;
		data->Root.CurrentCluster = data->Root.FirstCluster;
	}
	else
	{
		data->OpenedFiles[file->Handle].Opened = false;	
	}
}

bool FatFindFile(Disk* disk, FatFile far* file, const char* name, FatDirectoryEntry* entryOut)
{
	char fatName[12];
	FatDirectoryEntry entry;

	// convert from name to fat name
	MemorySet(fatName, ' ', sizeof(fatName));
	fatName[11] = '\0';

	const char* ext = StringChar(name, '.');
	if (ext == null)
		ext = name + 11;

	for (int i=0; i<8 && name[i] && name + i < ext; i++)
		fatName[i] = ToUpper(name[i]);

	if (ext != null)
	{
		for (int i=0; i < 3 && ext[i+1]; i++)
			fatName[i+8] = ToUpper(ext[i+1]);
	}

	while(FatReadEntry(disk, file, &entry))
	{
		if(MemoryCompare(fatName, entry.Name, 11) == 0)
		{
			*entryOut = entry;
			return true;
		}
	}
    return false;
}

FatFile far* FatOpen(Disk* disk, const char* path)
{
	char name[MAX_PATH_SIZE];

	// ignore leading slash
	if (path[0] ==	'/')
		path++;

	FatFile far* current = &data->Root.Public;

	while(*path)
	{
		// extract current file name from path 
		bool isLast = false;
		const char* delim = StringChar(path, '/');
		if (delim != null)
		{
			MemoryCopy(name, path, delim - path);
			name[delim - path + 1] = '\0';
			path = delim + 1;
		}
		else
		{
			unsigned len = StringLength(path);
			MemoryCopy(name, path, len);
			name[len + 1] = '\0';
			path += len;
			isLast = true;
		}

		// find directory entry in current directory
		FatDirectoryEntry entry;
		if (FatFindFile(disk, current, name, &entry))
		{
			FatClose(current);

			// check if directory
			if (!isLast && (entry.Attributes & FAT_ATTRIBUTE_DIRECTORY) == 0)
			{
				PrintFormat("FAT: %s not a directory!\r\n", name);
				return null;
			}

			// open new directory entry
			current = FatOpenEntry(disk, &entry);
		}
		else
		{
			FatClose(current);

			PrintFormat("FAT: %s not found!\r\n", name);
			return null;
		}
	}

	return current;
}
