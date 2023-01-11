#pragma once
#include <core/dev/CharacterDevice.hpp>
#include <stdint.h>
#include <stddef.h>

namespace Arch::I686::IO
{
	class VGATextDevice : public CharacterDevice
	{
		public:
			VGATextDevice();
			virtual size_t Read (      uint8_t* data, size_t size);
			virtual size_t Write(const uint8_t* data, size_t size);

			void 	Clear    ();
			//void SetCursorX(int  x);
			//void SetCursorY(int  y);
			//int  GetCursorX();
			//int  GetCursorY();

		private:
			void 	PutChar (int x, int y, char    c);
			void 	PutColor(int x, int y, uint8_t c);
			char 	GetChar (int x, int y);
			uint8_t GetColor(int x, int y);

			void 	SetCursor(int x, int  y);
			void 	Scroll   (int lines);
			
			void 	PutChar  (      char    c);

			//int32_t  m_cursorPos;
			int32_t  m_screenX;
			int32_t  m_screenY;
			static uint8_t* s_screenBuffer;
	};
}