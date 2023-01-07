#include "TextDevice.hpp"
#include <cpp/TypeTraits.hpp>

enum class FormatState
{
	Default			= 0,
	Length			= 1,
	LengthShort		= 2,
	LengthLong		= 3,
	Spec			= 4,  
};
enum class FormatLength
{
	Default			= 0,
	ShortShort		= 1,
	Short			= 2,
	Long			= 3,
	LongLong  		= 4,
};

const char TextDevice::s_hexChars[] = "0123456789abcdef";
TextDevice::TextDevice(CharacterDevice* dev)
: m_dev(dev)
{

}

bool TextDevice::Write (      char  c)
{
	return m_dev->Write(reinterpret_cast<const uint8_t*>(&c), sizeof(c)) == sizeof(c);
}
bool TextDevice::Write (const char* s)
{
	bool ok = true;
 	while(*s && ok)
	{
		ok = ok && Write(*s);
		s++;
	}
	return ok;
}
bool TextDevice::Format(const char* f, ...)
{
	bool ok = true;
	va_list args;
	va_start(args, f);
	ok = ok && Format(f, args);
	va_end(args);
	return ok;
}
bool TextDevice::Format(const char* f, va_list args)
{
	bool		 ok		 = true;
	FormatState  state   = FormatState::Default;
    FormatLength length  = FormatLength::Default;
    int          radix   = 10;
    bool 		 sign    = false;
    bool 		 number  = false;

    while (*f)
    {
        switch (state)
        {
            case FormatState::Default:
                switch (*f)
                {
                    case '%':   state = FormatState::Length;
                                break;
                    default:    ok = ok && Write(*f);
                                break;
                }
                break;

            case FormatState::Length:
                switch (*f)
                {
                    case 'h':   length = FormatLength::Short;
                                state  = FormatState::LengthShort;
                                break;
                    case 'l':   length = FormatLength::Long;
                                state  = FormatState::LengthLong;
                                break;
                    default:    goto PRINTF_STATE_SPEC;
                }
                break;

            case FormatState::LengthShort:
                if (*f == 'h')
                {
                    length = FormatLength::ShortShort;
                    state  = FormatState::Spec;
                }
                else goto PRINTF_STATE_SPEC;
                break;

            case FormatState::LengthLong:
                if (*f == 'l')
                {
                    length = FormatLength::LongLong;
                    state  = FormatState::Spec;
                }
                else goto PRINTF_STATE_SPEC;
                break;

            case FormatState::Spec:
            PRINTF_STATE_SPEC:
                switch (*f)
                {
                    case 'c':   ok = ok && Write((char)va_arg(args, int));
                                break;

                    case 's':   
                                ok = ok && Write((const char*)va_arg(args, const char*));
                                break;

                    case '%':   ok = ok && Write('%');
                                break;

                    case 'd':
                    case 'i':   radix = 10; sign = true; number = true;
                                break;

                    case 'u':   radix = 10; sign = false; number = true;
                                break;

                    case 'X':
                    case 'x':
                    case 'p':   radix = 16; sign = false; number = true;
                                break;

                    case 'o':   radix = 8; sign = false; number = true;
                                break;

                    // ignore invalid spec
                    default:    break;
                }

                if (number)
                {
                    if (sign)
                    {
                        switch (length)
                        {
                            case FormatLength::ShortShort:
                            case FormatLength::Short:
                            case FormatLength::Default:     ok = ok && Write<signed int>(va_arg(args, signed int), radix);
                                                            break;

                            case FormatLength::Long:        ok = ok && Write<signed long>(va_arg(args, signed long), radix);
                                                            break;

                            case FormatLength::LongLong:  	ok = ok && Write<signed long long>(va_arg(args, signed long long), radix);
                                                            break;
                        }
                    }
                    else
                    {
                        switch (length)
                        {
                            case FormatLength::ShortShort:
                            case FormatLength::Short:
                            case FormatLength::Default:     ok = ok && Write<unsigned int>(va_arg(args, unsigned int), radix);
                                                            break;
                                                            
                            case FormatLength::Long:        ok = ok && Write<unsigned long>(va_arg(args, unsigned long), radix);
                                                            break;

                            case FormatLength::LongLong:   	ok = ok && Write<unsigned long long>(va_arg(args, unsigned long long), radix);
                                                            break;
                        }
                    }
                }

                // reset state
                state   = FormatState::Default;
                length  = FormatLength::Default;
                radix   = 10;
                sign    = false;
                number  = false;
                break;
        }

        f++;
    }
	return ok;
}
bool TextDevice::FormatBuffer(const char* msg, const void* buffer, size_t count)
{
	bool ok = true;
	const uint8_t* u8Buffer = (const uint8_t*)buffer;
    
    ok = ok && Write(msg);
    for (uint16_t i = 0; i < count; i++)
    {
        ok = ok && Write(s_hexChars[u8Buffer[i] >> 4]);
        ok = ok && Write(s_hexChars[u8Buffer[i] & 0xF]);
    }
    ok = ok && Write("\n");
	return ok;
}


template<typename TNumber>
bool TextDevice::Write(TNumber value, int radix)
{
	bool ok = true;
	typename MakeUnsigned<TNumber>::Type number;
	if (IsSigned<TNumber>() && value < 0)
	{
		ok = ok && Write('-');
		number = -value;
	}
	else
	{
		number =  value;
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