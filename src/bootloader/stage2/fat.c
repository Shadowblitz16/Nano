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
#define FAT_CACHE_SIZE			5

typedef struct
{
	// extended boot record
	uint8_t 	DriveNumber;
	uint8_t		Reserved;
	uint8_t 	Signature;
	uint32_t	VolumnID;
	uint8_t  	VolumnLabel[11];
	uint8_t		SystemID[8];

} __attribute__((packed)) Fat_ExtendedBootRecord1216;

typedef struct
{
	uint32_t SectorsPerFat;
	uint16_t Flags;
	uint16_t FatVersion;
	uint32_t RootDirectoryCluster;
	uint16_t FSInfoSector;
	uint16_t BackupBootSector;
	uint8_t  Reserved[12];
	Fat_ExtendedBootRecord1216 EBR;
} __attribute__((packed)) Fat_ExtendedBootRecord32;

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

	union
	{
		Fat_ExtendedBootRecord1216	EBR1216;
		Fat_ExtendedBootRecord32 	EBR32;
	};
	
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
	uint8_t		 FatCache[FAT_CACHE_SIZE * SECTOR_SIZE];
	uint32_t     FatCachePosition;

	uint8_t  	 FatLFNOrder[FAT_LFN_LAST];
	uint16_t 	 FatLFNCharacters[FAT_LFN_LAST][13];
	int 		 FatLFNCount;

} Fat_Data;
static Fat_Data* g_data 			= NULL;
static uint32_t  g_dataSectionLBA 	= 0;
static uint8_t	 g_fatType        	= 0;
static uint32_t  g_totalSectors   	= 0;
static uint32_t	 g_sectorsPerFat	= 0;

bool Fat_ReadBootSector(Partition* partition)
{
	return MBR_ReadSectors(partition, 0, 1, g_data->BS.BootSectorBytes);
}

bool Fat_ReadFat(Partition* partition, size_t lbaIndex)
{
	return MBR_ReadSectors(partition, g_data->BS.BootSector.ReservedSectors + lbaIndex, FAT_CACHE_SIZE, g_data->FatCache);
}

void Fat_Detect(Partition* partition)
{
	uint32_t dataClusters = (g_totalSectors - g_dataSectionLBA) / g_data->BS.BootSector.SectorsPerCluster;
	if		(dataClusters < 0xFF5) 						g_fatType = 12;
	else if (g_data->BS.BootSector.SectorsPerFat != 0) 	g_fatType = 16;
	else 												g_fatType = 32;
}

uint32_t  Fat_ClusterToLBA(uint32_t cluster);

bool Fat_Init(Partition* partition)
{
	g_data = (Fat_Data*)MEMORY_FAT_ADDR;

	// read boot sector
	if(!Fat_ReadBootSector(partition))
	{
		printf("FAT: read boot sector failed!\r\n");
		return false;
	}

	// read FAT
	g_data->FatCachePosition = 0xFFFFFFFF;
	g_totalSectors = g_data->BS.BootSector.TotalSectors;
	if (g_totalSectors == 0) { // fat32 
		g_totalSectors = g_data->BS.BootSector.LargeSectorCount;
	}

	bool isFat32 = false;
	g_sectorsPerFat = g_data->BS.BootSector.SectorsPerFat;
	if (g_sectorsPerFat == 0) { // fat32
		isFat32 = true;
		g_sectorsPerFat = g_data->BS.BootSector.EBR32.SectorsPerFat;
	}

	// read root directory
	uint32_t rootDirLBA	 	= 0;
	uint32_t rootDirSize	= 0;

	if (isFat32)
	{
		g_dataSectionLBA = g_data->BS.BootSector.ReservedSectors + g_sectorsPerFat * g_data->BS.BootSector.FatCount;

		rootDirLBA  	 = Fat_ClusterToLBA(g_data->BS.BootSector.EBR32.RootDirectoryCluster);
		rootDirSize      = 0;
	} 
	else         
	{
		rootDirLBA  	 = g_data->BS.BootSector.ReservedSectors + g_sectorsPerFat * g_data->BS.BootSector.FatCount;
		rootDirSize 	 = sizeof(Fat_DirectoryEntry) * g_data->BS.BootSector.DirEntryCount;	
		uint32_t rootDirSectors = (rootDirSize + g_data->BS.BootSector.BytesPerSector - 1) / g_data->BS.BootSector.BytesPerSector;
		g_dataSectionLBA = rootDirLBA + rootDirSectors;
	}

	// open opened root directory file
	g_data->Root.Public.Handle 			= ROOT_DIRECTORY_HANDLE;
	g_data->Root.Public.IsDirectory		= true;
	g_data->Root.Public.Position		= 0;
	g_data->Root.Public.Size 			= sizeof(Fat_DirectoryEntry) * g_data->BS.BootSector.DirEntryCount;
	g_data->Root.Opened 				= true;
	g_data->Root.FirstCluster			= rootDirLBA;
	g_data->Root.CurrentCluster			= rootDirLBA;
	g_data->Root.CurrentSectorInCluster	= 0;

	if (!MBR_ReadSectors(partition, rootDirLBA, 1, g_data->Root.Buffer))
	{
		printf("FAT: read root directory failed!\r\n");
		return false;	
	}

	// calulate data section
	Fat_Detect(partition);

	// reset opened files
	for (int i=0; i<MAX_FILE_HANDLES; i++)
		g_data->OpenedFiles[i].Opened = false;

	g_data->FatLFNCount = 0;
	return true;
}

uint32_t  Fat_ClusterToLBA(uint32_t cluster)
{
	return g_dataSectionLBA + (cluster - 2) * g_data->BS.BootSector.SectorsPerCluster;
}

Fat_File* Fat_OpenEntry(Partition* partition, Fat_DirectoryEntry* entry)
{
	// find empty handle
	int handle = -1;
	for (int i=0; i<MAX_FILE_HANDLES && handle < 0; i++)
	{
		if (!g_data->OpenedFiles[i].Opened)
			handle = i;
	}

	// out of handles
	if (handle < 0)
	{
		printf("FAT: out of file handles!\r\n");
		return false;	
	}

	// setup vars
	Fat_FileData* fd 			= &g_data->OpenedFiles[handle];
	fd->Public.Handle 			= handle;
	fd->Public.IsDirectory 		= (entry->Attributes & FAT_ATTRIBUTE_DIRECTORY) != 0;
	fd->Public.Position     	= 0;
	fd->Public.Size         	= entry->Size;
	fd->FirstCluster			= entry->FirstClusterLow + ((uint32_t)entry->FirstClusterHigh << 16);
	fd->CurrentCluster      	= fd->FirstCluster;
	fd->CurrentSectorInCluster 	= 0;

	if (!MBR_ReadSectors(partition, Fat_ClusterToLBA(fd->CurrentCluster), 1, fd->Buffer))
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

uint32_t Fat_NextCluster(Partition* partition, uint32_t currentCluster)
{
	// Determine the byte offset of the entry we need to read
	uint32_t fatIndex = currentCluster;
	if 		(g_fatType == 12) 	fatIndex = currentCluster * 3 / 2;
	else if (g_fatType == 16)	fatIndex = currentCluster * 2;
	else /*if (fatType == 32)*/	fatIndex = currentCluster * 4;

	// Make sure cahce has the right number
	uint32_t fatIndexSector = fatIndex / SECTOR_SIZE;
	if (fatIndexSector < g_data->FatCachePosition || fatIndexSector >= g_data->FatCachePosition + FAT_CACHE_SIZE)
	{
		Fat_ReadFat(partition, fatIndexSector);
		g_data->FatCachePosition = fatIndexSector;
	}
	
	
	fatIndex -= g_data->FatCachePosition * SECTOR_SIZE;
	uint32_t nextCluster = 0;;
	if 		(g_fatType == 12)
	{
		if (currentCluster % 2 == 0) nextCluster = (*(uint16_t*)(g_data->FatCache + fatIndex)) & 0x0FFF;
		else 					 	 nextCluster = (*(uint16_t*)(g_data->FatCache + fatIndex)) >> 4;

		if (nextCluster >= 0x00000FF8) 
			nextCluster |= 0xFFFFF000;
	}
	else if (g_fatType == 16)
	{
		nextCluster = (*(uint16_t*)(g_data->FatCache + fatIndex));
		
		if (nextCluster >= 0x0000FFF8) 
			nextCluster |= 0xFFFF0000;
	
	}
	else /*if (fatType == 32)*/
	{
		nextCluster = (*(uint32_t*)(g_data->FatCache + fatIndex));
	}

	printf("fatType=%d, fatIndex=%x, fatIndexSector=%x, nextCluster=%x\n", g_fatType, fatIndex, fatIndexSector, nextCluster);
	return nextCluster;
}

uint32_t Fat_Read(Partition* partition, Fat_File* file, uint32_t byteCount, void* dataOut)
{
	// get file data
	Fat_FileData* fd = (file->Handle == ROOT_DIRECTORY_HANDLE) ? &g_data->Root : &g_data->OpenedFiles[file->Handle];

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
				if (!MBR_ReadSectors(partition, fd->CurrentCluster, 1, fd->Buffer))
				{
					printf("FAT: read error!\r\n");
					break;
				}
			}
			else
			{
				// calculate next cluster & sectors to read
				if (++fd->CurrentSectorInCluster >= g_data->BS.BootSector.SectorsPerCluster)
				{
					// read next cluster
					fd->CurrentSectorInCluster 	= 0;
					fd->CurrentCluster 			= Fat_NextCluster(partition, fd->CurrentCluster);
				}

				if (fd->CurrentCluster >= 0xFFFFFFF8)
				{
					fd->Public.Size = fd->Public.Position;
					break;
				}

				// read next sector
				if (!MBR_ReadSectors(partition, Fat_ClusterToLBA(fd->CurrentCluster) + fd->CurrentSectorInCluster, 1, fd->Buffer))
				{
					printf("FAT: read error!\r\n");
					break;
				}
			}
		}
	}

	return u8DataOut - (uint8_t*)dataOut;
}

bool 	 	 Fat_ReadEntry(Partition* partition, Fat_File* file, Fat_DirectoryEntry* dirEntry)
{
	return Fat_Read(partition, file, sizeof(Fat_DirectoryEntry), dirEntry) == sizeof(Fat_DirectoryEntry);
}

void 	 	 Fat_Close(Fat_File* file)
{
	if (file->Handle == ROOT_DIRECTORY_HANDLE)
    {
		file->Position = 0;
		g_data->Root.CurrentCluster = g_data->Root.FirstCluster;
	}
	else
	{
		g_data->OpenedFiles[file->Handle].Opened = false;	
	}
}

void Fat_GetShortName(const char* name, char shortName[12])
{

	// convert from name to fat name
	memset(shortName, ' ', 12);
	shortName[11] = '\0';

	const char* ext = strchr(name, '.');
	if (ext == NULL)
		ext = name + 11;

	for (int i=0; i<8 && name[i] && name + i < ext; i++)
		shortName[i] = toupper(name[i]);

	if (ext != name + 11)
	{
		for (int i=0; i < 3 && ext[i+1]; i++)
			shortName[i+8] = toupper(ext[i+1]);
	}

}

bool Fat_FindFile(Partition* partition, Fat_File* file, const char* name, Fat_DirectoryEntry* entryOut)
{
	char fatName[12];
	Fat_DirectoryEntry entry;

	Fat_GetShortName(name, fatName);

	while(Fat_ReadEntry(partition, file, &entry))
	{
		if (entry.Attributes == FAT_ATTRIBUTE_LFN) 
		{
			Fat_LongFileEntry* lfn = (Fat_LongFileEntry*)&entry;

			int idx 	= g_data->FatLFNCount++;
			int offset 	= 0;
			
			g_data->FatLFNOrder[idx] = lfn->Order & (FAT_LFN_LAST);
			memcpy(g_data->FatLFNCharacters[idx] + 0 , lfn->Chars1, sizeof(lfn->Chars1));
			memcpy(g_data->FatLFNCharacters[idx] + 5 , lfn->Chars2, sizeof(lfn->Chars2));
			memcpy(g_data->FatLFNCharacters[idx] + 11, lfn->Chars3, sizeof(lfn->Chars3));

			// is this the last LFN block
			if ((lfn->Order & FAT_LFN_LAST) != 0)
			{
				
			}
		}

		if(memcmp(fatName, entry.Name, 11) == 0)
		{
			*entryOut = entry;
			return true;
		}
	}
    return false;
}

Fat_File* Fat_Open(Partition* partition, const char* path)
{
	char name[MAX_PATH_SIZE];

	// ignore leading slash
	if (path[0] ==	'/')
		path++;

	Fat_File* current = &g_data->Root.Public;

	while(*path)
	{
		// extract current file name from path 
		bool isLast = false;
		const char* delim = strchr(path, '/');
		if (delim != NULL)
		{
			memcpy(name, path, delim - path);
			name[delim - path] = '\0';
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
		printf("Searching for: %s",name);

		// find directory entry in current directory
		Fat_DirectoryEntry entry;
		if (Fat_FindFile(partition, current, name, &entry))
		{
			Fat_Close(current);

			// check if directory
			if (!isLast && (entry.Attributes & FAT_ATTRIBUTE_DIRECTORY) == 0)
			{
				printf("FAT: %s not a directory!\r\n", name);
				return NULL;
			}

			// open new directory entry
			current = Fat_OpenEntry(partition, &entry);
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
