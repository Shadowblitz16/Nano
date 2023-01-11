#include "Assert.hpp"
#include <core/arch/i686/IO.hpp>
#include <Debug.hpp>
bool Assert_(const char* condition, const char* filename, int line, const char* function)
{
	Debug::Critical("Assert", "Assert failed: %s", condition);
	Debug::Critical("Assert", "In file %s line %d function %s", filename, line, function);
	Arch::I686::IO::Panic();
	return false;
}