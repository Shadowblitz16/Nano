#pragma once
#include <stddef.h>

const char* strchr(const char* string, char character);
char* 		strcpy(char* destination, const char* source);
unsigned 	strlen(const char* string);
int 		strcmp(const char* string1, const char* string2);

wchar_t* utf16_to_codepoint(wchar_t* string, int* codepoint);
char* codepoint_to_utf16(int codepoint, char* stringOutput);