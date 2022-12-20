#pragma once
#include "StdInt.h"

void _cdecl x86Div(u64 dividend, u32 divisor, u64* quotientOut, u32* remainderOut);
void _cdecl x86VideoWriteCharTeletype(char c, u8 page);
bool _cdecl x86DiskReset(u8 drive);
bool _cdecl x86DiskRead(u8 drive, u16 cylinder, u16 sector, u16 head, u8 count, void far* dataOut);
bool _cdecl x86DiskGetDriveParams(u8 drive, u8* driveTypeOut, u16* cylindersOut, u16* sectorsOut, u16* headsOut);
