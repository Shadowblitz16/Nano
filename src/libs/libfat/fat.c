#include "fat.h"
#include <stddef.h>
#include "stdio.h"
#include "memdefs.h"
#include "string.h"
#include "memory.h"
#include "ctype.h"
#include "minmax.h"

#define SECTOR_SIZE 			512
#define MAX_PATH_SIZE        	256
#define MAX_FILE_HANDLES		10
#define ROOT_DIRECTORY_HANDLE	-1

typedef struct
{
	uint8_t 	BootJumpInstruction[3];
	uint8_t 	OEMIdentifier[8];
	uint16_t	BytesPerSector;
	uint8_t		SectorsPerCluster;
	uint16_t 	ReservedSectors;
	uint8_t 	FatCount;
	uint16_t	DirEntryCount;
	uint16_t	TotalSectors;
	uint8_t		MediaDescriptorType;    
	uint16_t	SectorsPerFat;
	uint16_t 	SectorsPerTrack;
	uint16_t 	Heads;
	uint32_t 	HiddenSectors;
    uint32_t 	LargeSectorCount;

	// extended boot record
	uint8_t 	DriveNumber;
	uint8_t		Reserved;
	uint8_t 	Signature;
	uint32_t	VolumnID;
	uint8_t  	VolumnLabel[11];
	uint8_t		SystemID[8];

	// We don't care about code.

} __attribute__((packed)) Fat_BootSector;

typedef struct
{
	uint8_t 	Buffer[SECTOR_SIZE];
	Fat_File 	Public;
	bool 		Opened;
	uint32_t    FirstCluster;
	uint32_t 	CurrentCluster;
	uint32_t 	CurrentSectorInCluster;
} Fat_FileData;

typedef struct 
{
	union
	{
		Fat_BootSector 	BootSector;
		uint8_t		 	BootSectorBytes[SECTOR_SIZE];
	} BS;
	
	Fat_FileData Root;
	Fat_FileData OpenedFiles[MAX_FILE_HANDLES];

} Fat_Data;
static Fat_Data* data 			= NULL;
static uint8_t*	 fat 			= NULL;
static uint32_t  dataSectionLBA = 0;


bool Fat_ReadBootSector(Disk* disk)
{
	return Disk_ReadSectors(disk, 0, 1, data->BS.BootSectorBytes);
}

bool Fat_ReadFat(Disk* disk)
{
	return Disk_ReadSectors(disk, data->BS.BootSector.ReservedSectors, data->BS.BootSector.SectorsPerFat, fat);
}

bool Fat_Init(Disk* disk)
{
	data = (Fat_Data*)MEMORY_FAT_ADDR;

	// read boot sector
	if(!Fat_ReadBootSector(disk))
	{
		printf("FAT: read boot sector failed!\r\n");
		return false;
	}

	// read FAT
	fat = (uint8_t*)data + sizeof(Fat_Data);
	uint32_t fatSize = data->BS.BootSector.BytesPerSector * data->BS.BootSector.SectorsPerFat;
	if (sizeof(Fat_Data) + fatSize >= MEMORY_FAT_SIZE)
	{
		printf("FAT: not enough memory to read FAT! Requires %lu, only have %u\r\n", sizeof(Fat_Data) + fatSize, MEMORY_FAT_SIZE);
		return false;
	}

	if (!Fat_ReadFat(disk))
	{
		printf("FAT: read fat failed!\r\n");
		return false;	
	}

	// read root directory
	uint32_t rootDirLBA	= data->BS.BootSector.ReservedSectors + data->BS.BootSector.SectorsPerFat * data->BS.BootSector.FatCount;
	uint32_t rootDirSize = sizeof(Fat_DirectoryEntry) * data->BS.BootSector.DirEntryCount;

	// open opened root directory file
	data->Root.Public.Handle 			= ROOT_DIRECTORY_HANDLE;
	data->Root.Public.IsDirectory		= true;
	data->Root.Public.Position			= 0;
	data->Root.Public.Size 				= sizeof(Fat_DirectoryEntry) * data->BS.BootSector.DirEntryCount;
	data->Root.Opened 					= true;
	data->Root.FirstCluster				= rootDirLBA;
	data->Root.CurrentCluster			= rootDirLBA;
	data->Root.CurrentSectorInCluster	= 0;

	if (!Disk_ReadSectors(disk, rootDirLBA, 1, data->Root.Buffer))
	{
		printf("FAT: read root directory failed!\r\n");
		return false;	
	}

	// calulate data section
	uint32_t rootDirSectors = (rootDirSize + data->BS.BootSector.BytesPerSector - 1) / data->BS.BootSector.BytesPerSector;
	dataSectionLBA =	rootDirLBA + rootDirSectors;

	// reset opened files
	for (int i=0; i<MAX_FILE_HANDLES; i++)
		data->OpenedFiles[i].Opened = false;

	return true;
}

uint32_t  Fat_ClusterToLBA(uint32_t cluster)
{
	return dataSectionLBA + (cluster - 2) * data->BS.BootSector.SectorsPerCluster;
}

Fat_File* Fat_OpenEntry(Disk* disk, Fat_DirectoryEntry* entry)
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
		printf("FAT: out of file handles!\r\n");
		return false;	
	}

	// setup vars
	Fat_FileData* fd 			= &data->OpenedFiles[handle];
	fd->Public.Handle 			= handle;
	fd->Public.IsDirectory 		= (entry->Attributes & FAT_ATTRIBUTE_DIRECTORY) != 0;
	fd->Public.Position     	= 0;
	fd->Public.Size         	= entry->Size;
	fd->FirstCluster			= entry->FirstClusterLow + ((uint32_t)entry->FirstClusterHigh << 16);
	fd->CurrentCluster      	= fd->FirstCluster;
	fd->CurrentSectorInCluster 	= 0;

	if (!Disk_ReadSectors(disk, Fat_ClusterToLBA(fd->CurrentCluster), 1, fd->Buffer))
	{
		printf("FAT: open entry failed - read error! cluster=%u lba=%u\n", fd->CurrentCluster, Fat_ClusterToLBA(fd->CurrentCluster));
		for (int i=0; i<11; i++)
			printf("%c", entry->Name[i]);
		printf("\n");
		return false;	
	}

	fd->Opened = true;
	return &fd->Public;
}

uint32_t Fat_NextCluster(uint32_t currentCluster)
{
	uint32_t fatIndex = currentCluster * 3 / 2;
	if (currentCluster % 2 == 0) return currentCluster = (*(uint16_t*)(fat + fatIndex)) & 0x0FFF;
	else 					 	 return currentCluster = (*(uint16_t*)(fat + fatIndex)) >> 4;
}

uint32_t Fat_Read(Disk* disk, Fat_File* file, uint32_t byteCount, void* dataOut)
{
	// get file data
	Fat_FileData* fd = (file->Handle == ROOT_DIRECTORY_HANDLE) ? &data->Root : &data->OpenedFiles[file->Handle];

	uint8_t*	u8DataOut = (uint8_t*)dataOut; 

	// don't read past the end of file
	if (!fd->Public.IsDirectory || (fd->Public.IsDirectory && fd->Public.Size != 0))
		byteCount = min(byteCount, fd->Public.Size - fd->Public.Position);

	while(byteCount > 0)
	{
		uint32_t left = SECTOR_SIZE - (fd->Public.Position % SECTOR_SIZE);
		uint32_t take = min(byteCount, left);

		memcpy(u8DataOut, fd->Buffer + fd->Public.Position % SECTOR_SIZE, take);
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
				if (!Disk_ReadSectors(disk, fd->CurrentCluster, 1, fd->Buffer))
				{
					printf("FAT: read error!\r\n");
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
					fd->CurrentCluster 			= Fat_NextCluster(fd->CurrentCluster);
				}

				if (fd->CurrentCluster >= 0xFF8)
				{
					fd->Public.Size = fd->Public.Position;
					break;
				}

				// read next sector
				if (!Disk_ReadSectors(disk, Fat_ClusterToLBA(fd->CurrentCluster) + fd->CurrentSectorInCluster, 1, fd->Buffer))
				{
					printf("FAT: read error!\r\n");
					break;
				}
			}
		}
	}

	return u8DataOut - (uint8_t*)dataOut;
}

bool 	 	 Fat_ReadEntry(Disk* disk, Fat_File* file, Fat_DirectoryEntry* dirEntry)
{
	return Fat_Read(disk, file, sizeof(Fat_DirectoryEntry), dirEntry) == sizeof(Fat_DirectoryEntry);
}

void 	 	 Fat_Close(Fat_File* file)
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

bool Fat_FindFile(Disk* disk, Fat_File* file, const char* name, Fat_DirectoryEntry* entryOut)
{
	char fatName[12];
	Fat_DirectoryEntry entry;

	// convert from name to fat name
	memset(fatName, ' ', sizeof(fatName));
	fatName[11] = '\0';

	const char* ext = strchr(name, '.');
	if (ext == NULL)
		ext = name + 11;

	for (int i=0; i<8 && name[i] && name + i < ext; i++)
		fatName[i] = toupper(name[i]);

	if (ext != name + 11)
	{
		for (int i=0; i < 3 && ext[i+1]; i++)
			fatName[i+8] = toupper(ext[i+1]);
	}

	while(Fat_ReadEntry(disk, file, &entry))
	{
		if(memcmp(fatName, entry.Name, 11) == 0)
		{
			*entryOut = entry;
			return true;
		}
	}
    return false;
}

Fat_File* Fat_Open(Disk* disk, const char* path)
{
	char name[MAX_PATH_SIZE];

	// ignore leading slash
	if (path[0] ==	'/')
		path++;

	Fat_File* current = &data->Root.Public;

	while(*path)
	{
		// extract current file name from path 
		bool isLast = false;
		const char* delim = strchr(path, '/');
		if (delim != NULL)
		{
			memcpy(name, path, delim - path);
			name[delim - path + 1] = '\0';
			path = delim + 1;
		}
		else
		{
			unsigned len = strlen(path);
			memcpy(name, path, len);
			name[len + 1] = '\0';
			path += len;
			isLast = true;
		}

		// find directory entry in current directory
		Fat_DirectoryEntry entry;
		if (Fat_FindFile(disk, current, name, &entry))
		{
			Fat_Close(current);

			// check if directory
			if (!isLast && (entry.Attributes & FAT_ATTRIBUTE_DIRECTORY) == 0)
			{
				printf("FAT: %s not a directory!\r\n", name);
				return NULL;
			}

			// open new directory entry
			current = Fat_OpenEntry(disk, &entry);
		}
		else
		{
			Fat_Close(current);

			printf("FAT: %s not found!\r\n", name);
			return NULL;
		}
	}

	return current;
}
