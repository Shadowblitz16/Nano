#pragma once
#include <core/Defs.hpp>

struct ExtendedDriveParams
{
    uint16_t ParamsSize;
    uint16_t Flags;
    uint32_t Cylinders;
    uint32_t Heads;
    uint32_t SectorsPerTrack;
    uint64_t Sectors;
    uint16_t BytesPerSector;
} __attribute__((packed));

struct ExtendedDriveData
{
    uint8_t  ParamsSize;
    uint8_t  Reserved;
    uint16_t Count;
    uint32_t Buffer;
    uint64_t LBA;
} __attribute__((packed));

EXPORT bool ASMCALL i686_Disk_Reset		   (uint8_t driveId);
EXPORT bool ASMCALL i686_Disk_HasExtensions(uint8_t driveId);
EXPORT bool ASMCALL i686_Disk_ReadDefaultDriveParams(
	uint8_t  				driveId,
	uint8_t*  				driveTypeOut,
	uint16_t* 				driveCylindersOut,
	uint16_t*				driveSectorsOut,
	uint16_t* 				driveHeadsOut);
EXPORT bool ASMCALL i686_Disk_ReadDefaultDriveData(
	uint8_t  				driveId,
	uint16_t 				driveCylinder,
	uint16_t 				driveSector,
	uint16_t 				driveHead,
	uint8_t  				driveCount,
	void* 	 				driveLowerDataOut);
EXPORT bool ASMCALL i686_Disk_ReadExtendedDriveParams(
	uint8_t	 			 	drive,
    ExtendedDriveParams* 	params);
EXPORT bool ASMCALL i686_Disk_ReadExtendedDriveData(
	uint8_t 				drive,
    ExtendedDriveData*  	params);
