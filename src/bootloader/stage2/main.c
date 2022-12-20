#include "StdBool.h"
#include "StdInt.h"
#include "StdIO.h"
#include "Fat.h"

void _cdecl cstart_(u16 bootDrive)
{
	Disk disk;
	if (!DiskInit(&disk, bootDrive))
	{
		PrintFormat("DISK: init failed\r\n");
		goto end;
	}
	

	if (!FatInit(&disk))
	{
		PrintFormat("FAT: init failed\r\n");
		goto end;
	}

	// browse files in root directory
	FatFile far* fd = FatOpen(&disk, "/");
	FatDirectoryEntry entry;
	int i=0;
	while (FatReadEntry(&disk, fd, &entry) && i++ < 5)
	{
		PrintFormat("    ");
		for (int i=0; i<11; i++)
			PutChar(entry.Name[i]);
		PrintFormat("\r\n");
	}
	FatClose(fd);

	char buffer[100];
	u32 read;
	fd = FatOpen(&disk, "mydir/test.txt");
	while((read = FatRead(&disk, fd, sizeof(buffer), buffer)))
	{
		for (u32 i=0; i<read; i++)
		{
			if (buffer[i] == '\n')
				PutChar('\r');
			PutChar(buffer[i]);
		}
	}
	FatClose(fd);

end:
	for (;;);
}
