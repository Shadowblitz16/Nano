#pragma once
#include <stdarg.h>
#include "CharacterDevice.hpp"
#include <core/cpp/TypeTraits.hpp>


class TextDevice 
{
	public: 
		TextDevice(CharacterDevice* dev);

		bool Write  (      char  chr);
		bool Write  (const char* str);
		bool VFormat(const char* fmt, va_list args);
		bool Format (const char* fmt, ...);
		bool FormatBuffer(const char* msg, const void* buffer, size_t count);


	private:
		CharacterDevice*  m_dev;
		static const char s_hexChars[];

		template<typename TNumber>
		bool Write(TNumber value, int radix);

};

template<typename TNumber>
bool TextDevice::Write(TNumber number, int radix)
{
	bool ok = true;
	typename MakeUnsigned<TNumber>::Type unsnumber;
	if (IsSigned<TNumber>() && number < 0)
	{
		ok = ok && Write('-');
		unsnumber = -number;
	}
	else
	{
		unsnumber =  number;
	}
	
	char buffer[32];
	int pos = 0;

	// Convert number to ASCII
	do
	{
		typename MakeUnsigned<TNumber>::Type rem = number % radix;
		number /= radix;
		buffer[pos++] = s_hexChars[rem];
	} while (number > 0);

	// Write number in reverse order
	while(--pos >= 0)
		ok = ok && Write(buffer[pos]);
	
	return ok;
}