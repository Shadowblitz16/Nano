#include "StdBool.h"
#include "StdInt.h"
#include "StdIo.h"

void _cdecl cstart_(u16 bootDrive)
{
	PutString("Hello world from C!", 20);
	for (;;);
}
