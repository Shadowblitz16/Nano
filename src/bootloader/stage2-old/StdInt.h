#pragma once

typedef signed   char 			s8;
typedef unsigned char 			u8;
typedef signed   short 			s16;
typedef unsigned short 			u16;
typedef signed   long int 		s32;
typedef unsigned long int 		u32;
typedef signed   long long int 	s64;
typedef unsigned long long int  u64;

#define null 		((void*)0)

#define Min(a,b)	(((a) < (b)) ? (a) : (b))
#define Max(a,b)	(((a) > (b)) ? (a) : (b))
