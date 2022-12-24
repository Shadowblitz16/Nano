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
