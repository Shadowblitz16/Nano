#include "vga_text.h"
#include "io.h"
static const unsigned SCREEN_WIDTH  = 80;
static const unsigned SCREEN_HEIGHT = 25;
static const uint8_t  DEFAULT_COLOR = 0x7;

static uint8_t* g_ScreenBuffer = (uint8_t*)0xB8000;
static int32_t  g_ScreenX      = 0;
static int32_t  g_ScreenY      = 0;

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


void    VGA_PutChr  	(int x, int y, char    c)
{
    g_ScreenBuffer[2 * (y * SCREEN_WIDTH + x)] = c;
}
void    VGA_PutCol		(int x, int y, uint8_t c)
{
    g_ScreenBuffer[2 * (y * SCREEN_WIDTH + x) + 1] = c; 
}
char    VGA_GetChr  	(int x, int y)
{
    return g_ScreenBuffer[2 * (y * SCREEN_WIDTH + x)];
}
uint8_t VGA_GetCol		(int x, int y)
{
    return g_ScreenBuffer[2 * (y * SCREEN_WIDTH + x) + 1]; 
}
void 	VGA_SetCursor	(int x, int y)
{
    int pos = y * SCREEN_WIDTH + x;

    i8259_outb(0x3D4, 0x0F);
    i8259_outb(0x3D5, (uint8_t)((pos >> 0) & 0xFF));

    i8259_outb(0x3D4, 0x0E);
    i8259_outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}
void 	VGA_Clear		()
{
    for (int y=0; y<SCREEN_HEIGHT; y++)
    for (int x=0; x<SCREEN_WIDTH; x++)
    {
        VGA_PutChr(x, y, '\0');
        VGA_PutCol(x, y, DEFAULT_COLOR);
    }
    g_ScreenX = 0;
    g_ScreenY = 0;
    VGA_SetCursor(g_ScreenX, g_ScreenY);
}
void 	VGA_ScrollBack  (int lines)
{
    for (int y = lines; y<SCREEN_HEIGHT; y++)
    for (int x = 0;     x<SCREEN_WIDTH;  x++)
    {
        VGA_PutChr(x, y - lines, VGA_GetChr(x,y));
        VGA_PutCol(x, y - lines, VGA_GetCol(x,y));
    }

    for (int y = SCREEN_HEIGHT - lines; y < SCREEN_HEIGHT; y++)
    for (int x = 0;                     x < SCREEN_WIDTH;  x++)    
    {
        VGA_PutChr(x, y, '\0');
        VGA_PutCol(x, y, DEFAULT_COLOR);   
    }

    g_ScreenY -= lines;
}
void 	VGA_PutC		(char        c  )
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
                VGA_PutC(' ');
            break;

        case '\r':
            g_ScreenX =  0;
            break;

        default:
            VGA_PutChr(g_ScreenX, g_ScreenY, c);
            g_ScreenX += 1;
            break;
    }

    if (g_ScreenX == SCREEN_WIDTH)
    {
        g_ScreenX  = 0;
        g_ScreenY += 1;
    }

    if (g_ScreenY >= SCREEN_HEIGHT)
        VGA_ScrollBack(1);
    
    VGA_SetCursor(g_ScreenX, g_ScreenY);
}
void 	VGA_PutS		(const char* str)
{
    while(*str)
    {
        VGA_PutC(*str);
        str++;
    }
}