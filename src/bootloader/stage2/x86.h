#pragma once
#include "StdInt.h"

void _cdecl x86Div(u64 dividend, u32 divisor, u64* quotientOut, u32* remainderOut);
void _cdecl x86VideoWriteCharTeletype(char c, u8 page);
