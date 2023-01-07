#include "x86.h"
#include "stdio.h"
#include <stdarg.h>
#include <stdbool.h>

const unsigned SCREEN_WIDTH  = 80;
const unsigned SCREEN_HEIGHT = 25;
const uint8_t  DEFAULT_COLOR = 0x7;

uint8_t* g_ScreenBuffer = (uint8_t*)0xB8000;
int32_t  g_ScreenX      = 0;
int32_t  g_ScreenY      = 0;


#define PRINTF_STATE_NORMAL         0
#define PRINTF_STATE_LENGTH         1
#define PRINTF_STATE_LENGTH_SHORT   2
#define PRINTF_STATE_LENGTH_LONG    3
#define PRINTF_STATE_SPEC           4

#define PRINTF_LENGTH_DEFAULT       0
#define PRINTF_LENGTH_SHORT_SHORT   1
#define PRINTF_LENGTH_SHORT         2
#define PRINTF_LENGTH_LONG          3
#define PRINTF_LENGTH_LONG_LONG     4


static void    putchr  (int x, int y, char    c)
{
    g_ScreenBuffer[2 * (y * SCREEN_WIDTH + x)] = c;
}
static void    putcolor(int x, int y, uint8_t c)
{
    g_ScreenBuffer[2 * (y * SCREEN_WIDTH + x) + 1] = c; 
}
static char    getchr  (int x, int y)
{
    return g_ScreenBuffer[2 * (y * SCREEN_WIDTH + x)];
}
static uint8_t getcolor(int x, int y)
{
    return g_ScreenBuffer[2 * (y * SCREEN_WIDTH + x) + 1]; 
}

static const char g_HexChars[] = "0123456789abcdef";
static void internal_scrollback  (int lines)
{
    for (int y = lines; y<SCREEN_HEIGHT; y++)
    for (int x = 0;     x<SCREEN_WIDTH;  x++)
    {
        putchr  (x, y - lines, getchr  (x,y));
        putcolor(x, y - lines, getcolor(x,y));
    }

    for (int y = SCREEN_HEIGHT - lines; y < SCREEN_HEIGHT; y++)
    for (int x = 0;                     x < SCREEN_WIDTH;  x++)    
    {
        putchr  (x, y, '\0');
        putcolor(x, y, DEFAULT_COLOR);   
    }

    g_ScreenY -= lines;
}
static void internal_putc        (void (*putcFunc)(char), char        c  )
{
    (*putcFunc)(c);
}
static void internal_puts        (void (*putcFunc)(char), const char* str)
{
    while(*str)
    {
        (*putcFunc)(*str);
        str++;
    }
}
static void internel_printf_uint (void (*putcFunc)(char), unsigned long long number, int radix)
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
        (*putcFunc)(buffer[pos]);

}
static void internel_printf_sint (void (*putcFunc)(char),   signed long long number, int radix)
{
    if (number < 0)
    {
        (*putcFunc)('-');
        internel_printf_uint(putcFunc, number, radix);
    }
    else internel_printf_uint(putcFunc, number, radix);
}
static void internel_printf      (void (*putcFunc)(char),           const char* fmt, va_list args)
{
    int  state  = PRINTF_STATE_NORMAL;
    int  length = PRINTF_LENGTH_DEFAULT;
    int  radix  = 10;
    bool sign   = false;
    bool number = false;

    while (*fmt)
    {
        switch (state)
        {
            case PRINTF_STATE_NORMAL:
                switch (*fmt)
                {
                    case '%':   state = PRINTF_STATE_LENGTH;
                                break;
                    default:    internal_putc(putcFunc, *fmt);
                                break;
                }
                break;

            case PRINTF_STATE_LENGTH:
                switch (*fmt)
                {
                    case 'h':   length = PRINTF_LENGTH_SHORT;
                                state  = PRINTF_STATE_LENGTH_SHORT;
                                break;
                    case 'l':   length = PRINTF_LENGTH_LONG;
                                state  = PRINTF_STATE_LENGTH_LONG;
                                break;
                    default:    goto PRINTF_STATE_SPEC_;
                }
                break;

            case PRINTF_STATE_LENGTH_SHORT:
                if (*fmt == 'h')
                {
                    length = PRINTF_LENGTH_SHORT_SHORT;
                    state  = PRINTF_STATE_SPEC;
                }
                else goto PRINTF_STATE_SPEC_;
                break;

            case PRINTF_STATE_LENGTH_LONG:
                if (*fmt == 'l')
                {
                    length = PRINTF_LENGTH_LONG_LONG;
                    state  = PRINTF_STATE_SPEC;
                }
                else goto PRINTF_STATE_SPEC_;
                break;

            case PRINTF_STATE_SPEC:
            PRINTF_STATE_SPEC_:
                switch (*fmt)
                {
                    case 'c':   internal_putc(putcFunc, (char)va_arg(args, int));
                                break;

                    case 's':   internal_puts(putcFunc, va_arg(args, const char*));
                                break;

                    case '%':   internal_putc(putcFunc, '%');
                                break;

                    case 'd':
                    case 'i':   radix   = 10; 
                                sign    = true;
                                number  = true;
                                break;

                    case 'u':   radix   = 10; 
                                sign    = false;
                                number  = true;
                                break;
                    case 'X':
                    case 'x':
                    case 'p':   radix   = 16; 
                                sign    = false;
                                number  = true;
                                break;

                    case 'o':   radix   = 8; 
                                sign    = false;
                                number  = true;
                                break;

                    // ignore invalid spec
                    default:    break;
                }

                if (number)
                {
                    if (sign) 
                    {
                        switch (length)
                        {
                            case PRINTF_LENGTH_SHORT_SHORT  :
                            case PRINTF_LENGTH_SHORT        :
                            case PRINTF_LENGTH_DEFAULT      :
                                internel_printf_sint(putcFunc, va_arg(args, int      ), radix); 
                                break;
                            case PRINTF_LENGTH_LONG         :
                                internel_printf_sint(putcFunc, va_arg(args, long     ), radix);
                                break;
                            case PRINTF_LENGTH_LONG_LONG    :
                                internel_printf_sint(putcFunc, va_arg(args, long long), radix);
                                break;
                        }
                    }
                    else
                    {
                        switch (length)
                        {
                            case PRINTF_LENGTH_SHORT_SHORT  :
                            case PRINTF_LENGTH_SHORT        :
                            case PRINTF_LENGTH_DEFAULT      :
                                internel_printf_uint(putcFunc, va_arg(args, unsigned int       ), radix);
                                break;
                            case PRINTF_LENGTH_LONG         :
                                internel_printf_uint(putcFunc, va_arg(args, unsigned long      ), radix);
                                break;
                            case PRINTF_LENGTH_LONG_LONG    :
                                internel_printf_uint(putcFunc, va_arg(args, unsigned long long ), radix);
                                break;
                        }   
                    }
                }

                // reset state
                state   = PRINTF_STATE_NORMAL;
                length  = PRINTF_LENGTH_DEFAULT;
                radix   = 10;
                sign    = false;
                number  = false;
                break;
        }

        fmt++;
    }
}
static void internel_printb      (void (*putcFunc)(char), const char* msg, const void* buffer, uint32_t count)
{
    const uint8_t* u8Buffer = (const uint8_t*)buffer;
    internal_puts(putcFunc, msg);
    for (uint16_t i = 0; i < count; i++)
    {
        internal_putc(putcFunc, g_HexChars[u8Buffer[i] >> 4]);
        internal_putc(putcFunc, g_HexChars[u8Buffer[i] & 0xF]);
    }
    internal_puts(putcFunc, "\n");
}

void setcursor(int x, int y)
{
    int pos = y * SCREEN_WIDTH + x;
    x86_outb(0x3D4, 0x0F);
    x86_outb(0x3D5, (uint8_t)((pos >> 0) & 0xFF));

    x86_outb(0x3D4, 0x0E);
    x86_outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}

void clrscr()
{
    for (int y=0; y<SCREEN_HEIGHT; y++)
    for (int x=0; x<SCREEN_WIDTH; x++)
    {
        putchr  (x, y, '\0');
        putcolor(x, y, DEFAULT_COLOR);
    }
    g_ScreenX = 0;
    g_ScreenY = 0;
    setcursor(g_ScreenX, g_ScreenY);
}

void dbgc(char        c  )
{
    x86_outb(0xE9, c);
}
void dbgs(const char* str)
{
    while(*str)
    {
        dbgc(*str);
        str++;
    }
}
void putc(char        c  )
{
    //x86_outb(0xE9, c);
    switch (c)
    {
        case '\n':
            g_ScreenX  = 0;
            g_ScreenY += 1;
            break;

        case '\t':
            for (int i=0; i<4- (g_ScreenX % 4); i++)
                putc(' ');
            break;

        case '\r':
            g_ScreenX =  0;
            break;

        default:
            putchr(g_ScreenX, g_ScreenY, c);
            g_ScreenX += 1;
            break;
    }

    if (g_ScreenX == SCREEN_WIDTH)
    {
        g_ScreenX  = 0;
        g_ScreenY += 1;
    }

    if (g_ScreenY >= SCREEN_HEIGHT)
        internal_scrollback(1);
    
    setcursor(g_ScreenX, g_ScreenY);
}
void puts(const char* str)
{
    while(*str)
    {
        putc(*str);
        str++;
    }
}

void printf(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    internel_printf(putc, fmt, args);
    va_end(args);
}
void debugf(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    internel_printf(dbgc, fmt, args);
    va_end(args);
}
void printb(const char* msg, const void* buffer, uint32_t count)
{
    internel_printb(putc, msg, buffer, count);
}
void debugb(const char* msg, const void* buffer, uint32_t count)
{
    internel_printb(dbgc, msg, buffer, count);
}



