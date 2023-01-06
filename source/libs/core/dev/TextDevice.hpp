#pragma once
#include <stdarg.h>
#include "CharacterDevice.hpp"

class TextDevice 
{
	public: 
		TextDevice(CharacterDevice* dev);

		bool Write (      char  chr);
		bool Write (const char* str);
		bool Format(const char* fmt, ...);
		bool Format(const char* fmt, va_list args);
		bool FormatBuffer(const char* msg, const void* buffer, size_t count);


	private:
		CharacterDevice*  m_dev;
		static const char s_hexChars[];

		template<typename TNumber>
		bool Write(TNumber value, int radix);

};