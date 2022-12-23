#pragma once
#include "StdInt.h"


void PutChar  (char c);
void PutString(const char* str);
void PutStringFar(const char far* str);
void _cdecl PrintFormat(const char* format, ...);
