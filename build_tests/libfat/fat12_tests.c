#include "fat12_tests.h"
#include "../../src/testing/testing.h"
#include "../../src/libs/libfat/fat.h"
#include <assert.h>

void libfat_fat12_smoke_test()
{
	Disk disk;
	Fat_Init(&disk);
	
	assert(disk)
}
