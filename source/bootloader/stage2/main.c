#include <stdint.h>
#include "stdio.h"
#include "x86.h"
#include "disk.h"
#include "fat.h"
#include "memdefs.h"
#include "memory.h"
#include "mbr.h"
#include "elf.h"

uint8_t* KernelLoadBuffer = (uint8_t*)MEMORY_LOAD_KERNAL;
uint8_t* Kernel			  = (uint8_t*)MEMORY_KERNAL_ADDR;

typedef void (*KernelStart)();

void __attribute__((cdecl)) start(uint16_t bootDrive, void* partition)
{
	clrscr();

	printf("partition=%x\r\n", partition);

	Disk disk;
	if (!Disk_Init(&disk, bootDrive))
	{
		printf("DISK: init failed\r\n");
		goto end;
	}

	Partition part;
	MBR_DetectPartition(&part, &disk, partition);

	if (!Fat_Init(&part))
	{
	 	printf("FAT: init failed\r\n");
	 	goto end;
	}

	// load kernel
	KernelStart kernelEntry = (KernelStart)Kernel;
	if (!ELF_Read(&part, "/boot/kernel.elf", (void**)&kernelEntry))
	{
		printf("ELF read failed, booting halted!");
		goto end;
	}

	// execute kernel
	kernelEntry();

end:
	for (;;);
}
