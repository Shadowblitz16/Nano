#include <arch/i686/BIOSDisk.hpp>
#include <arch/i686/Memory.hpp>
#include <core/Debug.hpp>
#include <core/Defs.hpp>
#include <core/Assert.hpp>
#include <core/dev/TextDevice.hpp>
#include <core/dev/RangeBlockDevice.hpp>
#include <core/arch/i686/E9TextDevice.hpp>
#include <core/arch/i686/VGATextDevice.hpp>
#include <stdint.h>
#include <dev/MBR.hpp>

static Arch::I686::IO::VGATextDevice g_VGADevice;
static Arch::I686::IO::E9TextDevice  g_E9Device;


EXPORT void ASMCALL Start(uint16_t bootDrive, uint32_t* partition)
{

	g_VGADevice.Clear();
	g_VGADevice.Write((const uint8_t*)"abc", 3);

	TextDevice screen(&g_VGADevice);
	Debug::AddOutputDevice(&screen, Debug::Level::Debug, false);
	TextDevice debug (&g_E9Device);
	Debug::AddOutputDevice(&debug, Debug::Level::Debug, false);

	debug .Write('1');
	screen.Write('2');

	screen.Format("debug.Write: %d",  (int)debug .Write("debug.Write"));
	debug .Format("screen.Write: %d", (int)screen.Write("screen.Write"));

	debug.Format("Hello, debug! %d", 1);
	screen.Format("Hello, world! %d", 2);


	BIOSDisk disk(bootDrive);
	if (!disk.Init())
	{
		Debug::Critical("stage2", "Failed to initialize disk!");
	}

	// Handle partitioned disks
	BlockDevice* part;
	RangeBlockDevice partRange;
	if (bootDrive < 0x80)
	{
		part = &disk;
	}
	else
	{
		MBR* entry = ToLinear<MBR*>(part);
		partRange(&disk, entry->LbaStart, entry->Size);
		part = &partRange;
	}

	Debug::Info("stage2", "OK!");

	for (;;);
}