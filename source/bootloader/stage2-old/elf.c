#include "elf.h"
#include "fat.h"
#include "stdio.h"
#include "memdefs.h"
#include "memory.h"
#include "minmax.h"


bool ELF_Read(Partition* part, const char* path, void** entryPoint)
{

	uint8_t* headerBuffer = MEMORY_ELF_ADDR;
	uint8_t* loadBuffer   = MEMORY_LOAD_KERNAL;
	uint32_t filePos = 0;
	uint32_t read = 0;

	// Read header
	Fat_File* file = Fat_Open(part, path);
	if ((read = Fat_Read(part, file, sizeof(ELFHeader), headerBuffer)) != sizeof(ELFHeader))
	{
		printf("ELF Load error!\n");
		return false;
	}
	filePos += read;

	// validate header
	bool ok = true;
	ELFHeader* header = (ELFHeader*)headerBuffer;
	ok = ok && (memcmp(header->Magic, ELF_MAGIC, 4) != 0);
	ok = ok && (header->Bitness 		== ELF_BITNESS_32BIT);
	ok = ok && (header->Endianness 		== ELF_ENDIANNESS_LITTLE);
	ok = ok && (header->HeaderVersion 	== 1);
	ok = ok && (header->ELFVersion 		== 1);
	ok = ok && (header->Type 			== ELF_TYPE_EXECUTABLE);
	ok = ok && (header->InstructionSet 	== ELF_INSTRUCTIONSET_X86);
	
	*entryPoint = (void*)header->ProgramEntryPosition;

	// load program header
	uint32_t programHeaderOffset 			= header->ProgramHeaderTablePosition;
	uint32_t programHeaderSize				= header->ProgramHeaderTableEntrySize * header->ProgramHeaderTableEntryCount;
	uint32_t programHeaderTableEntrySize 	= header->ProgramHeaderTableEntrySize;
	uint32_t programHeaderTableEntryCount 	= header->ProgramHeaderTableEntryCount;

	read += Fat_Read(part, file, programHeaderOffset - filePos, headerBuffer);
	if ((read = Fat_Read(part, file, programHeaderSize, headerBuffer)) != programHeaderSize)
	{
		printf("ELF Load error!\n");
		return false;
	}
	filePos += read;
	Fat_Close(file);

	// parse program header entries
	for (uint32_t i=0; i<programHeaderTableEntryCount; i++)
	{
		ELF_ProgramHeader* programHeader = (ELF_ProgramHeader*)(headerBuffer + i * programHeaderTableEntrySize);
		if (programHeader->Type  == ELF_PROGRAM_TYPE_LOAD)
		{

			// TODO: validate that the program doesn't overwrite the stage2
			
			uint8_t* virtualAddress = (uint8_t*)programHeader->VirtualAddress;
			memset(virtualAddress, 0, programHeader->MemorySize);
			
			// ugly nasty seeking
			// TODO: proper seeking
			file = Fat_Open(part, path);
			while(programHeader->Offset > 0)
			{
				uint32_t shouldRead = min(programHeader->Offset, MEMORY_LOAD_SIZE);
				read = Fat_Read(part, file, shouldRead, loadBuffer);
				if (read != shouldRead)
				{
					printf("ELF load error!\n");
					return false;
				}
				programHeader->Offset -= read;
			}

			// read program
			while(programHeader->FileSize > 0)
			{
				uint32_t shouldRead = min(programHeader->FileSize, MEMORY_LOAD_SIZE);
				read = Fat_Read(part, file, shouldRead, loadBuffer);
				if (read != shouldRead)
				{
					printf("ELF load error!\n");
					return false;
				}
				programHeader->FileSize -= read;

				memcpy(virtualAddress, loadBuffer, read);
				virtualAddress += read;
			}

			Fat_Close(file);
		}	
	}

	return true;
}
