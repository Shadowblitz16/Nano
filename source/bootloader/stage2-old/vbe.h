#pragma once
#include <stdint.h>
#include <stdbool.h>

typedef struct {
	char VbeSignature[4];
	uint16_t VbeVersion;
	uint16_t OemStringPtr[2];
	uint8_t  Capabilities[4];
	uint32_t VideoModePtr;
	uint16_t TotalMemory;
	uint8_t  Reserved[236+256];
} __attribute__((packed)) VbeInfoBlock;

typedef struct {
	uint16_t Attributes;		// deprecated, only bit 7 should be of interest to you, and it indicates the mode supports a linear frame buffer.
	uint8_t windowA;			// deprecated
	uint8_t WindowB;			// deprecated
	uint16_t Granularity;		// deprecated; used while calculating bank numbers
	uint16_t WindowSize;
	uint16_t SegmentA;
	uint16_t SegmentB;
	uint32_t WinFuncPtr;		// deprecated; used to switch banks from protected mode without returning to real mode
	uint16_t Pitch;			// number of bytes per horizontal line
	uint16_t Width;			// width in pixels
	uint16_t Height;			// height in pixels
	uint8_t WChar;			// unused...
	uint8_t YChar;			// ...
	uint8_t Planes;
	uint8_t Bpp;			// bits per pixel in this mode
	uint8_t Banks;			// deprecated; total number of banks in this mode
	uint8_t MemoryModel;
	uint8_t BankSize;		// deprecated; size of a bank, almost always 64 KB but may be 16 KB...
	uint8_t ImagePages;
	uint8_t Reserved0;
 
	uint8_t RedMask;
	uint8_t RedPosition;
	uint8_t GreenMask;
	uint8_t GreenPosition;
	uint8_t BlueMask;
	uint8_t BluePosition;
	uint8_t ReservedMask;
	uint8_t ReservedPosition;
	uint8_t DirectColorAttributes;
 
	uint32_t Framebuffer;		// physical address of the linear frame buffer; write here to draw to the screen
	uint32_t OffScreenMemOff;
	uint16_t OffScreenMemSize;	// size of memory in the framebuffer but not being displayed on the screen
	uint8_t Reserved1[206];
} __attribute__ ((packed)) VbeModeInfo;

bool VBE_GetControllerInfo(VbeInfoBlock* info);
bool VBE_GetModeInfo(uint16_t mode, VbeModeInfo* info);
bool VBE_SetMode(uint16_t mode);