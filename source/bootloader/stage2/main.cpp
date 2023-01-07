#include <core/Defs.hpp>
#include <core/dev/TextDevice.hpp>
#include <core/arch/i686/VGATextDevice.hpp>
#include <stdint.h>

static Arch::I686::VGATextDevice g_vgaDevice;
EXPORT void ASMCALL Start(uint16_t bootDrive, void* partition)
{

	g_vgaDevice.Clear();

	TextDevice screen(&g_vgaDevice);
	screen.Format("Hello, world! %d", 2);

	for (;;);
}