#include <stdint.h>
#include "stdio.h"
#include "disk.h"
#include "x86.h"

void* g_data = (void*)0x20000;
void __attribute__((cdecl)) start(uint16_t bootDrive)
{
	clrscr();

	Disk disk;
	if (!Disk_Init(&disk, bootDrive))
	{
		printf("DISK: init failed\r\n");
		goto end;
	}

	Disk_ReadSectors(&disk, 0, 1, g_data);
	print_buffer("Boot sector: ", g_data, 512);
	

	// if (!FatInit(&disk))
	// {
	// 	PrintFormat("FAT: init failed\r\n");
	// 	goto end;
	// }

	// // browse files in root directory
	// FatFile far* fd = FatOpen(&disk, "/");
	// FatDirectoryEntry entry;
	// int i=0;
	// while (FatReadEntry(&disk, fd, &entry) && i++ < 5)
	// {
	// 	PrintFormat("    ");
	// 	for (int i=0; i<11; i++)
	// 		PutChar(entry.Name[i]);
	// 	PrintFormat("\r\n");
	// }
	// FatClose(fd);

	// char buffer[100];
	// u32 read;
	// fd = FatOpen(&disk, "mydir/test.txt");
	// while((read = FatRead(&disk, fd, sizeof(buffer), buffer)))
	// {
	// 	for (u32 i=0; i<read; i++)
	// 	{
	// 		if (buffer[i] == '\n')
	// 			PutChar('\r');
	// 		PutChar(buffer[i]);
	// 	}
	// }
	// FatClose(fd);

end:
	for (;;);
}
