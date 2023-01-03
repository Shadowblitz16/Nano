#pragma once
#include "mbr.h"
#include <stdint.h>
#include "stdbool.h"

#define ELF_MAGIC ("\x7F" "ELF")

typedef struct {
	uint8_t 	Magic[4];
	uint8_t 	Bitness;			// 1 = 32 bit, 2 = 64 bit
	uint8_t 	Endianness;			// 1 = little endian, 2 = big endian
	uint8_t 	HeaderVersion;
	uint8_t 	ABI;
	uint8_t 	Padding1[8];
	uint16_t 	Type;				// 1 relocatable, 2 = executable, 3 = shared, 4 = core
	uint16_t 	InstructionSet;
	uint32_t    ELFVersion;
	uint32_t    ProgramEntryPosition;
	uint32_t    ProgramHeaderTablePosition;
	uint32_t    SectionHeaderTablePosition;
	uint32_t    Flags;
	uint16_t    HeaderSize;
	uint16_t    ProgramHeaderTableEntrySize;
	uint16_t    ProgramHeaderTableEntryCount;
	uint16_t    SectionHeaderTableEntrySize;
	uint16_t    SectionHeaderTableEntryCount;
	uint16_t    SectionNamesIndex;
} __attribute__((packed)) ELFHeader;

typedef enum {
	ELF_BITNESS_32BIT 			= 1,
	ELF_BITNESS_64BIT 			= 3
} ELFBitness;

typedef enum {
	ELF_ENDIANNESS_LITTLE 		= 1,
	ELF_ENDIANNESS_BIG 			= 2
} ELFEndianness;

typedef enum {
	ELF_INSTRUCTIONSET_NONE 	= 0,
	ELF_INSTRUCTIONSET_X86 		= 3,
	ELF_INSTRUCTIONSET_ARM 		= 0x28,
	ELF_INSTRUCTIONSET_X64 		= 0x3E,
	ELF_INSTRUCTIONSET_ARM64 	= 0xB7,
	ELF_INSTRUCTIONSET_RISCV 	= 0xF3
} ELFInstructionSet;

typedef enum {
	ELF_TYPE_RELOCATABLE 		= 1,
	ELF_TYPE_EXECUTABLE 		= 2,
	ELF_TYPE_SHARED 	 		= 3,
	ELF_TYPE_CORE 				= 4
} ELFType;

typedef struct 
{
	uint32_t Type;
	uint32_t Offset;
	uint32_t VirtualAddress;
	uint32_t PhysicalAddress;
	uint32_t FileSize;
	uint32_t MemorySize;
	uint32_t Flags;
	uint32_t Align;
} ELF_ProgramHeader;

typedef enum {
	ELF_PROGRAM_TYPE_NULL 				= 0x00000000,	// Program header table entry unused.
	ELF_PROGRAM_TYPE_LOAD 				= 0x00000001,	// Loadable segment.
	ELF_PROGRAM_TYPE_DYNAMIC 	 		= 0x00000002,	// Dynamic linking information.
	ELF_PROGRAM_TYPE_INTERP 			= 0x00000003,	// Interpreter information. 
	ELF_PROGRAM_TYPE_NOTE 				= 0x00000004,	// Auxiliary information.
	ELF_PROGRAM_TYPE_SHLIB 				= 0x00000005,	// Reserved
	ELF_PROGRAM_TYPE_PHDR 				= 0x00000006,	// Segment containing program header tables itself.
	ELF_PROGRAM_TYPE_TLS 				= 0x00000007,	// Thread-Local storage template
	ELF_PROGRAM_TYPE_LOOS 				= 0x60000000,	// Reserved inclusive range. Operating system specific.
	ELF_PROGRAM_TYPE_HIOS 				= 0x6FFFFFFF,
	ELF_PROGRAM_TYPE_LOPROC 			= 0x70000000,   // Reserved inclusive range. Processor specific.
	ELF_PROGRAM_TYPE_HIPROC 			= 0x7FFFFFFF
} ELFProgramType;

bool ELF_Read(Partition* part, const char* path, void** entryPoint);