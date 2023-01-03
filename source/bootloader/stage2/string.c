#include "string.h"
#include <stddef.h>
#include <stdint.h>

const char* strchr(const char* string, char character)
{
	if (string == NULL) return NULL;	

	while(*string)
	{
		if (*string == character)
			return string;

		++string;
	}

	return NULL;
}
char* 		strcpy(char* destination, const char* source)
{
	char* originalDestination = destination;

	if (destination == NULL)
        return NULL;

	if (source == NULL)
	{
		*destination = '\0';
		return destination;
	}

	while(*source)
	{
		*destination = *source;
		++source;
		++destination;
	}

	*destination = '\0';
    return originalDestination;
}

unsigned 		strlen(const char* string)
{
	unsigned len = 0;
	while(*string)
	{
		++len;
		++string;
	}
	return len;
}

int strcmp(const char* string1, const char* string2)
{
	const unsigned char *s1 = (const unsigned char *) string1;
	const unsigned char *s2 = (const unsigned char *) string2;
	unsigned char c1, c2;

	do
	{
		c1 = (unsigned char) *s1++;
		c2 = (unsigned char) *s2++;
		if (c1 == '\0') return c1 - c2;
	}
	while (c1 == c2);
	return c1 - c2;
}

wchar_t* utf16_to_codepoint(wchar_t* string, int* codepoint)
{
	int c1 = *string;
	++string;

	if (c1 >= 0xd800 && c1 < 0xdc00)
	{
		int c2 = *string;
		++string;

		*codepoint = ((c1 & 0x3ff) << 10) + (c2 & 0x3ff) + 0x10000;
	}
	*codepoint = c1;

	return string;
}
char* codepoint_to_utf16(int codepoint, char* stringOutput)
{
	if (codepoint <= 0x7F) {
		*stringOutput = (char)codepoint;
	}
	else if (codepoint <= 0x7FF)
	{
		*stringOutput++ = 0xC0 | ((codepoint >> 6) & 0x1F);
		*stringOutput++ = 0x80 | (codepoint & 0x3F);
	}
	else if (codepoint <= 0xFFFF) {
		*stringOutput++ = 0xE0 | ((codepoint >> 12) & 0xF);
		*stringOutput++ = 0x80 | ((codepoint >> 6 ) & 0x3F);
		*stringOutput++ = 0x80 | (codepoint & 0x3F);	
	}
	else if (codepoint <= 0x1FFFFF)
	{
		*stringOutput++ = 0xF0 | ((codepoint >> 18) & 0x7);
		*stringOutput++ = 0x80 | ((codepoint >> 12) & 0x3F);
		*stringOutput++ = 0x80 | ((codepoint >> 6 ) & 0x3F);
		*stringOutput++ = 0x80 | (codepoint & 0x3F);		
	}

	return stringOutput;
}