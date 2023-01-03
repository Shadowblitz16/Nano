#include "stdio.h"
#include "stdint.h"
#include "stdlib.h"
#include "string.h"
#include "memory.h"
#include <hal/hal.h>

extern uint8_t __bss_start;
extern uint8_t __end;
extern void _init();

void crash_me();

const char* my_array[] = 
{
	"hello",
	"alpha",
	"zzzz",
	"aaaa",
	"cccc",
	"wqerwerwerwerwerwer"
};
int my_compare(const void* a, const void* b)
{
	const char* a_int = *(const char**)a;
	const char* b_int = *(const char**)b;
	return strcmp(a_int, b_int);
}

void start(uint16_t bootDrive)
{
	// call global constructors
	_init();

	HAL_Init();

	clrscr();
	//printf("%d \n", 0);
	//printf("%d \n", 1);

	printf("Nanobyte OS\n");
	printf("This operating system is under construction.\n");

	crash_me();

	// printf("%d ", sizeof(my_array) / sizeof(my_array[0]));
	// qsort(my_array, sizeof(my_array) / sizeof(my_array[0]), sizeof(my_array[0]), my_compare);
	// for (int i = 0; i < sizeof(my_array) / sizeof(my_array[0]); i+=1) 
	// 	printf("%s ", my_array[i]);
	// printf("\n");

end:
	for (;;);
}