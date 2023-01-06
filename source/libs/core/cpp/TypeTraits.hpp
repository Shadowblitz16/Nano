#pragma once

template<typename T>
constexpr bool IsSigned();

// signed
template<>	constexpr bool IsSigned<char>				() { return true; }
template<>	constexpr bool IsSigned<short>				() { return true; }
template<>	constexpr bool IsSigned<int>				() { return true; }
template<>	constexpr bool IsSigned<long>				() { return true; }
template<>	constexpr bool IsSigned<long long>			() { return true; }

// unsigned
template<>	constexpr bool IsSigned<unsigned char>		() { return false; }
template<>	constexpr bool IsSigned<unsigned short>		() { return false; }
template<>	constexpr bool IsSigned<unsigned int>		() { return false; }
template<>	constexpr bool IsSigned<unsigned long>		() { return false; }
template<>	constexpr bool IsSigned<unsigned long long>	() { return false; }

template<typename T>
struct MakeUnsigned
{
	typedef T Type;
};

template<> 
struct MakeUnsigned<char>
{
	typedef unsigned char Type;
};

template<> 
struct MakeUnsigned<short>
{
	typedef unsigned short Type;
};

template<> 
struct MakeUnsigned<int>
{
	typedef unsigned int Type;
};

template<> 
struct MakeUnsigned<long>
{
	typedef unsigned long Type;
};

template<> 
struct MakeUnsigned<long long>
{
	typedef unsigned long long Type;
};



