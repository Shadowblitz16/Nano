#include "Debug.hpp"

#define MAX_OUTPUT_DEVICES 10

namespace
{
	static const char* const g_LogSeverityColors[] =
	{
       	[static_cast<int>(Debug::Level::Debug)]        = "\033[2;37m",
        [static_cast<int>(Debug::Level::Info)]         = "\033[37m",
        [static_cast<int>(Debug::Level::Warning)]      = "\033[1;33m",
        [static_cast<int>(Debug::Level::Error)]        = "\033[1;31m",
        [static_cast<int>(Debug::Level::Critical)]     = "\033[1;37;41m",
	};

	static const char* const g_ColorReset = "\033[0m";

	struct
	{
		TextDevice*  logDevice;
		Debug::Level logLevel;
		bool 		 logColor;
	} g_OutputDevices[MAX_OUTPUT_DEVICES];
	
	int g_OutputDevicesCount = 0;
}

namespace Debug
{

	static void Log(const char* module, Level logLevel, const char* fmt, va_list args)
	{
		for (int i = 0; i < g_OutputDevicesCount; i++)
		{
			if (logLevel < g_OutputDevices[i].logLevel)
				continue;

			if (g_OutputDevices[i].logColor)
				g_OutputDevices[i].logDevice->Write(g_LogSeverityColors[static_cast<int>(logLevel)]);

			g_OutputDevices[i].logDevice->Format("[%s], ", module);
			g_OutputDevices[i].logDevice->VFormat(fmt, args);

			if (g_OutputDevices[i].logColor)
				g_OutputDevices[i].logDevice->Write(g_ColorReset);

			g_OutputDevices[i].logDevice->Write('\n');
		}
	}

	void AddOutputDevice(TextDevice* logDevice, Debug::Level logLevel, bool logColor)
	{
		if (g_OutputDevicesCount >= MAX_OUTPUT_DEVICES)
		{
			Error("Debug.cpp", "Too many output devices!");
			return;
		}

		g_OutputDevices[g_OutputDevicesCount].logDevice = logDevice;
		g_OutputDevices[g_OutputDevicesCount].logLevel  = logLevel;
		g_OutputDevices[g_OutputDevicesCount].logColor  = logColor;
		g_OutputDevicesCount++;
	}

	void Debug   (const char* module, const char* fmt, ...)
	{
		va_list args;
		va_start(args, fmt);
		Log(module, Level::Debug, fmt, args);
		va_end(args);
	}
	void Info    (const char* module, const char* fmt, ...)
	{
		va_list args;
		va_start(args, fmt);
		Log(module, Level::Info, fmt, args);
		va_end(args);
	}
	void Warning (const char* module, const char* fmt, ...)
	{
		va_list args;
		va_start(args, fmt);
		Log(module, Level::Warning, fmt, args);
		va_end(args);
	}
	void Error   (const char* module, const char* fmt, ...)
	{
		va_list args;
		va_start(args, fmt);
		Log(module, Level::Error, fmt, args);
		va_end(args);
	}
	void Critical(const char* module, const char* fmt, ...)
	{
		va_list args;
		va_start(args, fmt);
		Log(module, Level::Critical, fmt, args);
		va_end(args);
	}
}