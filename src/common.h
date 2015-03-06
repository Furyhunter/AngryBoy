#pragma once

#include <string>

#define CPU_CLOCKRATE 4194304


#define FORCEINLINE __forceinline

typedef signed int     s32;
typedef signed short   s16;
typedef signed char     s8;
typedef unsigned int   u32;
typedef unsigned short u16;
typedef char            u8;

#define BETWEEN(x,l,u) x >= l && x < u
