#include <stdint.h>
#include "stdio.h"
#include "x86.h"
#include "disk.h"
#include "fat.h"
#include "memdefs.h"
#include "memory.h"

uint8_t* KernelLoadBuffer = (uint8_t*)MEMORY_LOAD_KERNAL;
uint8_t* Kernel			  = (uint8_t*)MEMORY_KERNAL_ADDR;

typedef void (*KernelStart)();

void __attribute__((cdecl)) start(uint16_t bootDrive)
{
	clrscr();

	Disk disk;
	if (!Disk_Init(&disk, bootDrive))
	{
		printf("DISK: init failed\r\n");
		goto end;
	}

	if (!Fat_Init(&disk))
	{
	 	printf("FAT: init failed\r\n");
	 	goto end;
	}

	// bload kernel
	Fat_File* 	file = Fat_Open(&disk, "/kernel.bin");
	if (!file)
    {
		printf("FAT: could not load /kernel.bin\n");
		goto end;
	}
	
	uint32_t 	read = 0;
	uint8_t*  	kernelBuffer = Kernel;
	while ((read = Fat_Read(&disk, file, MEMORY_LOAD_SIZE, KernelLoadBuffer)))
	{
		memcpy(kernelBuffer, KernelLoadBuffer, read);
		kernelBuffer += read;
	}
	Fat_Close(file);

	// execute kernel
	KernelStart kernelStart = (KernelStart)Kernel;
	kernelStart();

end:
	for (;;);
}
