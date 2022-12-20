#include "String.h"
#include "StdInt.h"

const char* StringChar(const char* string, char character)
{
	if (string == null) return null;	

	while(*string)
	{
		if (*string == character)
			return string;

		++string;
	}

	return null;
}
char* 		StringCopy(char* destination, const char* source)
{
	char* originalDestination = destination;

	if (destination == null)
        return null;

	if (source == null)
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

unsigned 		StringLength(const char* string)
{
	unsigned len = 0;
	while(*string)
	{
		++len;
		++string;
	}
	return len;
}
