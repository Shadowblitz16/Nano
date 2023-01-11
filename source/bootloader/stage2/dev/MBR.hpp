#pragma once

typedef struct 
{
	uint8_t Attributes;
	uint8_t ChrStart[3];
	uint8_t PartitionType;
	uint8_t ChsEnd[3];
	uint32_t LbaStrart;
	uint32_t Size;
} __attribute__((packed)) MBR