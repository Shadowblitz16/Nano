#include <stdint.h>
#include "stdio.h"
#include "disk.h"
#include "fat.h"
#include "memdefs.h"
#include "memory.h"
#include "mbr.h"
#include "elf.h"
#include "vbe.h"

uint8_t* KernelLoadBuffer = (uint8_t*)MEMORY_LOAD_KERNAL;
uint8_t* Kernel			  = (uint8_t*)MEMORY_KERNAL_ADDR;

typedef void (*KernelStart)();

#define COLOR(r,g,b) ((b) | (g << 8) | (r << 16))

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

	// const int desiredWidth  = 1024;
	// const int desiredHeight = 768;
	// const int desiredBpp    = 32;
	// uint16_t pickedMode 	= 0xFFFF;

	// // initialize graphics
	// VbeInfoBlock* vbeInfo =  (VbeInfoBlock*)MEMORY_VESA_INFO;
	// VbeModeInfo*  modeInfo = (VbeModeInfo*)MEMORY_MODE_INFO;
	// if (VBE_GetControllerInfo(vbeInfo))
	// {
	// 	uint16_t* mode = (uint16_t*)(vbeInfo->VideoModePtr);
	// 	for (int i=0; mode[i]!=0xFFFF; i++)
	// 	{
	// 		printf("Found VBE mode %x\r\n", mode[i]);
	// 		if (!VBE_GetModeInfo(mode[i], modeInfo)) 
	// 		{
	// 			//printf("Can't get mode info %x :(\n", mode[i]);
	// 			continue;
	// 		}

	// 		bool hasFB = (modeInfo->Attributes & 0x90) == 0x90; 
	// 		if (hasFB && desiredWidth==modeInfo->Width && desiredHeight==modeInfo->Height && desiredBpp==modeInfo->Bpp)
	// 		{
	// 			printf("Found mode %x %dx%dx%d 0x%x\n", mode[i], modeInfo->Width, modeInfo->Height, modeInfo->Bpp, modeInfo->Framebuffer);
	// 			pickedMode = mode[i];
	// 			break;
	// 		}
	// 	}
			
	// 	if (pickedMode != 0xFFFF && VBE_SetMode(pickedMode))
	// 	{
	// 		uint32_t* fb = (uint32_t*)(modeInfo->Framebuffer);
	// 		int w = modeInfo->Width;
	// 		int h = modeInfo->Height;

	// 		for (int y = 0; y < h; y++)	
	// 		for (int x = 0; x < w; x++)
	// 		{
	// 			fb[y * modeInfo->Pitch / 4 + x] = COLOR(x,y,x+y);
	// 		}
	// 	}
	// }
	// else
	// {
	// 	printf("No VBE extensions :(\n");
	// }

	//printf("Hello1 %x\n", 0x1234);
	//debugf("Hello2 %x\n", 0x1234);

	// execute kernel
	kernelEntry();

end:
	for (;;);
}
