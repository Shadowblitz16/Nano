#include <Defs.hpp>
#include <stdint.h>

namespace Arch::I686 
{
	EXPORT void    ASMCALL Out(uint16_t port, uint8_t value);
	EXPORT uint8_t ASMCALL In (uint16_t port);
}