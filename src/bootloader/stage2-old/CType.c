#include "CType.h"

char IsLower(char character)
{
	return character >= 'a' && character <= 'z';
}
char ToUpper(char character)
{
	return IsLower(character) ? (character - 'a' + 'A') : character;
}
