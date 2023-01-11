#pragma once
#include <core/dev/TextDevice.hpp>
namespace Debug
{
    enum class Level
    {
        Debug    = 0,
        Info     = 1,
        Warning  = 2,
        Error    = 3,
        Critical = 4
    };

	void AddOutputDevice(TextDevice* logDevice, Level logLevel, bool logColor);

	void Debug   (const char* module, const char* fmt, ...);
	void Info    (const char* module, const char* fmt, ...);
	void Warning (const char* module, const char* fmt, ...);
	void Error   (const char* module, const char* fmt, ...);
	void Critical(const char* module, const char* fmt, ...);
}