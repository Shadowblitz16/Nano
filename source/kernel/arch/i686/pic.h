#pragma once
#include <stdint.h>
#include <stdbool.h>


typedef struct {
	const char* Name;
	void (*Init)(uint8_t offsetPic1, uint8_t offsetPic2, bool autoEoi);
	bool (*Probe)();
	void (*Mask)(int irq);
	void (*Unmask)(int irq);
	void (*Disable)();
	void (*SendEndOfInterrupt)(int irq);

	
} PICDriver;