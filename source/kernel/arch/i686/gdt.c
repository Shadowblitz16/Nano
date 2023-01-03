#include "gdt.h"
#include <stdint.h>

typedef struct {
	uint16_t LimitLow;		// limit (bits 0-15)
	uint16_t BaseLow;		// base (bits 0-15)
	uint8_t  BaseMiddle;	// base (bits 16-23)
	uint8_t  Access;		// access
	uint8_t  FlagsLimitHi;  // limit (bits 16-19)
	uint8_t  BaseHigh;		// base (bits 24-31)
} __attribute__((packed)) GDTEntry;

typedef struct {
	uint16_t Limit;		// sizeof(gdt) - 1
	GDTEntry* Ptr;		// address of GDT
} __attribute__((packed)) GDTDescriptor;

typedef enum {
	GDT_ACCESS_CODE_READABLE  			= 0x02,
	GDT_ACCESS_DATA_WRITEABLE 			= 0x02,

	GDT_ACCESS_CODE_CONFORMING  		= 0x04,
	GDT_ACCESS_DATA_DIRECTION_NORMAL 	= 0x00,
	GDT_ACCESS_DATA_DIRECTION_DOWN 		= 0x04,

	GDT_ACCESS_CODE_SEGMENT		  		= 0x18,
	GDT_ACCESS_DATA_SEGMENT 			= 0x10,

	GDT_ACCESS_RING0		 			= 0x00,
	GDT_ACCESS_RING1 					= 0x20,
	GDT_ACCESS_RING2		 			= 0x40,
	GDT_ACCESS_RING3		 			= 0x60,

	GDT_ACCESS_PRESENT		 			= 0x80,
} GDT_ACCESS;

typedef enum {
	GDT_FLAG_64BIT 						= 0x20,
	GDT_FLAG_32BIT 						= 0x40,
	GDT_FLAG_16BIT 						= 0x00,

	GDT_FLAG_GRANULARITY_1B 			= 0x00,
	GDT_FLAG_GRANULARITY_4K 			= 0x80,
} GDT_FLAGS;

// Helper macros
#define GDT_LIMIT_LOW(limit) ((limit >> 00) & 0xFFFF)
#define GDT_BASE_LOW(base)  ((base  >> 00) & 0xFFFF)
#define GDT_BASE_MID(base)  ((base  >> 16) & 0xFF  )
#define GDT_BASE_HIGH(base)  ((base  >> 24) & 0xFF  )

#define GDT_FLAGS_LIMIT_HI(limit, flags)  ((limit >> 16) & 0xF) | (flags & 0xF0)

#define GDT_ENTRY(base, limit, access, flags) { \
	GDT_LIMIT_LOW	  (limit), 					\
	GDT_BASE_LOW	  (base ), 					\
	GDT_BASE_MID	  (base ), 					\
	access,										\
	GDT_FLAGS_LIMIT_HI(limit, flags), 			\
	GDT_BASE_HIGH	  (base)					\
}

GDTEntry 		g_GDT[] 		= {

	GDT_ENTRY(0,0,0,0),				// Null descriptor
	GDT_ENTRY						// Kernel 32 bit code segment
	(
		0,
		0xFFFFF,
		(
			GDT_ACCESS_PRESENT 			| 
			GDT_ACCESS_RING0 			| 
			GDT_ACCESS_CODE_SEGMENT 	| 
			GDT_ACCESS_CODE_READABLE
		),
		(
			GDT_FLAG_32BIT				|
			GDT_FLAG_GRANULARITY_4K
		)
	),	 
	GDT_ENTRY						// Kernel 32 bit data segment
	(
		0,
		0xFFFFF,
		(
			GDT_ACCESS_PRESENT 			| 
			GDT_ACCESS_RING0 			| 
			GDT_ACCESS_DATA_SEGMENT 	| 
			GDT_ACCESS_DATA_WRITEABLE
		),
		(
			GDT_FLAG_32BIT				|
			GDT_FLAG_GRANULARITY_4K
		)
	)
};
GDTDescriptor	g_GDTDescriptor = { 
	sizeof(g_GDT) - 1, 
	g_GDT
};

void __attribute__((cdecl)) i686_GDT_Load(GDTDescriptor* descriptor, uint16_t codeSegment, uint16_t dataSegment);

void i686_GDT_Init()
{
	i686_GDT_Load(&g_GDTDescriptor, i686_GDT_CODE_SEGMENT, i686_GDT_DATA_SEGMENT);
}