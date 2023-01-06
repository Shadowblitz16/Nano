#include "arch/i686/io.h"
#include "stdio.h"
#include <stdarg.h>
#include <stdbool.h>


const unsigned SCREEN_WIDTH  = 80;
const unsigned SCREEN_HEIGHT = 25;
const uint8_t  DEFAULT_COLOR = 0x7;

uint8_t* g_ScreenBuffer = (uint8_t*)0xB8000;
int32_t  g_ScreenX      = 0;
int32_t  g_ScreenY      = 0;

const char g_HexChars[] = "0123456789abcdef";
void fputc      (fd_t file,       char  chr)
{
    VFS_Write(file, &chr, sizeof(chr));   
}
void fputs      (fd_t file, const char* str)
{
    while(*str) 
    {
        fputc(file, *str);
        str++;
    } 
}
void fprintfu   (fd_t file, unsigned long long number, int radix)
{
    char buffer[32];
    int pos = 0;

    // convert number to ASCII
    do 
    {
        unsigned long long rem = number % radix;
        number /= radix;
        buffer[pos++] = g_HexChars[rem];
    } while (number > 0);

    // print number in reverse order
    while (--pos >= 0)
        fputc(file, buffer[pos]);
}
void fprintfs   (fd_t file,   signed long long number, int radix)
{
    if (number < 0)
    {
        fputc(file, '-');
        fprintfu(file,-number, radix);
    }
    else fprintfu(file, number, radix);
}
void fprintfv   (fd_t file, const char* fmt, va_list args)
{

}
void fprintf    (fd_t file, const char* fmt,          ...)
{
    va_list args;
    va_start(args, fmt);
    fprintfv(file, fmt, args);
    va_end  (args);
}
void fprintb    (fd_t file, const char* msg, const void* buffer, uint32_t count)
{
    const uint8_t* u8Buffer = (const uint8_t*)buffer;
    
    fputs(file, msg);
    for (uint16_t i = 0; i < count; i++)
    {
        fputc(file, g_HexChars[u8Buffer[i] >> 4]);
        fputc(file, g_HexChars[u8Buffer[i] & 0xF]);
    }
    fputs(file, "\n");
}

void dputc  (	   char  chr)
{
    fputc(VFS_FD_STDDBG, chr);
}
void dputs  (const char* str)
{
    fputs(VFS_FD_STDDBG, str);
}
void dprintf(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    fprintfv(VFS_FD_STDDBG, fmt, args);
    va_end  (args);
}
void dprintb(const char* msg, const void* buffer, uint32_t count)
{
    fprintb(VFS_FD_STDDBG, msg, buffer, count);
}

void putc  (      char  chr)
{
    fputc(chr, VFS_FD_STDOUT);
}
void puts  (const char* str)
{
    fputs(VFS_FD_STDOUT, str);
}
void printf(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    fprintfv(VFS_FD_STDOUT, fmt, args);
    va_end  (args);
}
void printb(const char* msg, const void* buffer, uint32_t count)
{
    fprintb(VFS_FD_STDOUT, msg, buffer, count);
}

