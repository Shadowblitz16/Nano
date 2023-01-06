#pragma once
#include <stdint.h>
#include <stdarg.h>
#include <hal/vfs.h>

void fputc   (fd_t file,       char  chr);
void fputs   (fd_t file, const char* str);
void fprintf (fd_t file, const char* fmt, 		   ...);
void fprintb (fd_t file, const char* msg, const void* buffer, uint32_t count);

void dputc  (	   char  chr);
void dputs  (const char* str);
void dprintf(const char* fmt, ...);
void dprintb(const char* msg, const void* buffer, uint32_t count);

void putc  (	  char  chr);
void puts  (const char* str);
void printf(const char* fmt, ...);
void printb(const char* msg, const void* buffer, uint32_t count);
