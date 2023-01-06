#include "VGATextDevice.hpp"
#include "IO.hpp"
#include <stdint.h>
namespace Arch::I686 
{
	static constexpr unsigned ScreenWidth  = 80;
	static constexpr unsigned ScreenHeight = 25;
	static constexpr uint8_t  DefaultColor = 0x7;

	static uint8_t* g_ScreenBuffer = (uint8_t*)0xB8000;

	// Public functions
	VGATextDevice::VGATextDevice()
		:
		m_CursorPos(0),
		m_ScreenX(0),
		m_ScreenY(0)
	{
		Clear();
	}

	size_t 	VGATextDevice::Read (      uint8_t* data, size_t size)
	{
		return 0;
	}
	size_t 	VGATextDevice::Write(const uint8_t* data, size_t size)
	{
		size_t i=0;
		for (; data[i] && i < size; i++)
			PutChar(data[i]);

		return i+1;
	}
	
	void    VGATextDevice::Clear ()
	{
		for (int y=0; y<ScreenHeight; y++)
		for (int x=0; x<ScreenWidth;  x++)
		{
			PutChar (x, y, '\0');
			PutColor(x, y, DefaultColor);
		}
		m_ScreenX = 0;
		m_ScreenY = 0;
		SetCursorX(m_ScreenX);
		SetCursorY(m_ScreenY);
	}
	void 	VGATextDevice::Scroll(unsigned int lines)
	{
		for (int y = lines; y<ScreenHeight; y++)
		for (int x = 0;     x<ScreenWidth;  x++)
		{
			PutChar (x, y - lines, GetChar (x,y));
			PutColor(x, y - lines, GetColor(x,y));
		}

		for (int y = ScreenHeight - lines; y < ScreenHeight; y++)
		for (int x = 0;                    x < ScreenWidth;  x++)    
		{
			PutChar (x, y, '\0');
			PutColor(x, y, DefaultColor);   
		}

		m_ScreenY -= lines;
	}
	
	void   	VGATextDevice::SetCursorX(int  x)
	{
    	m_CursorPos = m_ScreenY * ScreenWidth + x;

		Out(0x3D4, 0x0F);
		Out(0x3D5, (uint8_t)((m_CursorPos >> 0) & 0xFF));
	}
	void   	VGATextDevice::SetCursorY(int  y)
	{
    	m_CursorPos = y * ScreenWidth + m_ScreenX;
		
		Out(0x3D4, 0x0E);
		Out(0x3D5, (uint8_t)((m_CursorPos >> 8) & 0xFF));
	}
	int     VGATextDevice::GetCursorX()
	{
    	return m_CursorPos % ScreenWidth;
		
	}
	int    	VGATextDevice::GetCursorY()
	{
		return m_CursorPos / ScreenWidth;
	}
	
	// Private functions
	void 	VGATextDevice::PutChar  (int  x, int  y, char    c)
	{

		g_ScreenBuffer[2 * (y * ScreenWidth + x) + 0] = c;
	}
	void 	VGATextDevice::PutColor (int  x, int  y, uint8_t c)
	{
		g_ScreenBuffer[2 * (y * ScreenWidth + x) + 1] = c;
	}
	char 	VGATextDevice::GetChar  (int  x, int  y)
	{
		return g_ScreenBuffer[2 * (y * ScreenWidth + x) + 0];
	}
	uint8_t VGATextDevice::GetColor (int  x, int  y)
	{
		return g_ScreenBuffer[2 * (y * ScreenWidth + x) + 1];
	}

	void 	VGATextDevice::PutChar  (      char  c)
	{
		switch (c)
		{
			case '\n':
				m_ScreenX  = 0;
				m_ScreenY += 1;
				break;

			case '\t':
				for (int i=0; i<4- (m_ScreenX % 4); i++)
					PutChar(' ');
				break;

			case '\r':
				m_ScreenX =  0;
				break;

			default:
				PutChar(m_ScreenX, m_ScreenY, c);
				m_ScreenX += 1;
				break;
		}

		if (m_ScreenX == ScreenWidth)
		{
			m_ScreenX  = 0;
			m_ScreenY += 1;
		}

		if (m_ScreenY >= ScreenHeight)
			Scroll(1);
		
		SetCursorX(m_ScreenX);
		SetCursorY(m_ScreenY);
	}
	void 	VGATextDevice::PutString(const char* s)
	{
		while(*s)
		{
			PutChar(*s);
			s++;
		}
	}
}