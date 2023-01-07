#pragma once
#include <core/dev/CharacterDevice.hpp>
#include <stdint.h>
#include <stddef.h>
namespace Arch::I686 
{
	class VGATextDevice : public CharacterDevice
	{
		public:
			VGATextDevice();
			virtual size_t Read (      uint8_t* buffer, size_t size);
			virtual size_t Write(const uint8_t* buffer, size_t size);

			void Clear();
			void Scroll(unsigned int lines);

			void SetCursorX(int  x);
			void SetCursorY(int  y);
			int  GetCursorX();
			int  GetCursorY();

		private:
			void 	PutChar (int x, int y, char    c);
			void 	PutColor(int x, int y, uint8_t c);
			char 	GetChar (int x, int y);
			uint8_t GetColor(int x, int y);

			void 	PutChar  (      char    c);
			void 	PutString(const char*   c);

			int32_t  m_CursorPos;
			int32_t  m_ScreenX;
			int32_t  m_ScreenY;


	};
}