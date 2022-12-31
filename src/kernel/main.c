#include "stdio.h"
#include "stdint.h"
#include "stdlib.h"
#include "string.h"
#include "memory.h"

extern uint8_t __bss_start;
extern uint8_t __end;
extern void _init();

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

void __attribute__((section(".entry"))) start(uint16_t bootDrive)
{

	memset(&__bss_start, 0, (&__end) - (&__bss_start));

	// call global constructors
	_init();

	clrscr();
	printf("%d \n", 0);
	printf("%d \n", 1);

	printf("Hello, world from kernel!!!\n");
	
	printf("%d ", sizeof(my_array) / sizeof(my_array[0]));
	qsort(my_array, sizeof(my_array) / sizeof(my_array[0]), sizeof(my_array[0]), my_compare);
	for (int i = 0; i < sizeof(my_array) / sizeof(my_array[0]); i+=1) 
		printf("%s ", my_array[i]);
	printf("\n");

end:
	for (;;);
}
