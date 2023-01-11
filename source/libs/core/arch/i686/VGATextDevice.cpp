#include "VGATextDevice.hpp"
#include "IO.hpp"

namespace Arch::I686::IO
{
	static constexpr unsigned ScreenWidth  = 80;
	static constexpr unsigned ScreenHeight = 25;
	static constexpr uint8_t  DefaultColor = 0x7;

	uint8_t* VGATextDevice::s_screenBuffer = (uint8_t*)0xB8000;

	// Public functions
	VGATextDevice::VGATextDevice()
		:
		m_screenX(0),
		m_screenY(0)
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
		while(data[i] && i < size)
		{
			PutChar(data[i]);
			i++;
		}
		return i;
	}
	
	void    VGATextDevice::Clear ()
	{
		for (int y=0; y<ScreenHeight; y++)
		for (int x=0; x<ScreenWidth;  x++)
		{
			PutChar (x, y, '\0');
			PutColor(x, y, DefaultColor);
		}
		m_screenX = 0;
		m_screenY = 0;
		SetCursor(m_screenX, m_screenY);
	}

	// Private functions
	void 	VGATextDevice::PutChar  (int  x, int  y, char    c)
	{
		s_screenBuffer[2 * (y * ScreenWidth + x) + 0] = c;
	}
	void 	VGATextDevice::PutColor (int  x, int  y, uint8_t c)
	{
		s_screenBuffer[2 * (y * ScreenWidth + x) + 1] = c;
	}
	char 	VGATextDevice::GetChar  (int  x, int  y)
	{
		return s_screenBuffer[2 * (y * ScreenWidth + x) + 0];
	}
	uint8_t VGATextDevice::GetColor (int  x, int  y)
	{
		return s_screenBuffer[2 * (y * ScreenWidth + x) + 1];
	}

	void   	VGATextDevice::SetCursor(int  x, int  y)
	{
    	int pos = y * ScreenWidth + x;

		Out(0x3D4, 0x0F);
		Out(0x3D5, (uint8_t)((pos >> 0) & 0xFF));

		Out(0x3D4, 0x0E);
		Out(0x3D5, (uint8_t)((pos >> 8) & 0xFF));		
	}
	void 	VGATextDevice::Scroll   (int lines)
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

		m_screenY -= lines;
	}

	void 	VGATextDevice::PutChar  (      char  c)
	{
		switch (c)
		{
			case '\n':
				m_screenX  = 0;
				m_screenY += 1;
				break;

			case '\t':
				for (int i = 0; i < 4 - (m_screenX % 4); i++)
					PutChar(' ');
				break;

			case '\r':
				m_screenX =  0;
				break;

			default:
				PutChar(m_screenX, m_screenY, c);
				m_screenX += 1;
				break;
		}

		if (m_screenX >= ScreenWidth)
		{
			m_screenY += 1;
			m_screenX  = 0;
		}

		if (m_screenY >= ScreenHeight)
			Scroll(1);
		
		SetCursor(m_screenX, m_screenY);
		//SetCursorX(m_screenX);
		//SetCursorY(m_screenY);
	}
}