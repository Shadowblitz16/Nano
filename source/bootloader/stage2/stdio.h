#pragma once
#include <stdint.h>

void setcursor (int x, int y);
void clrscr();
void dbgc  (char c);
void dbgs  (const char* str);
void putc  (char c);
void puts  (const char* str);
void printf(const char* format, ...);
void printb(const char* msg, const void* buffer, uint32_t count);
void debugf(const char* format, ...);
void debugb(const char* msg, const void* buffer, uint32_t count);
