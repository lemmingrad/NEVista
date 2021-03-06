#ifndef _TYPES_H_
#define _TYPES_H_
#pragma once

//----------------------------------------------------------//
// TYPES.H
//----------------------------------------------------------//
//-- Description			
// Useful typedefs, includes, etc.
//----------------------------------------------------------//


#include <cstdio> 
#include <cstdlib>
#include <cstdarg>
#include <cassert>


//----------------------------------------------------------//
// DEFINES
//----------------------------------------------------------//


//-- bool as integer or bool?
//#define BOOL_AS_INT

#if defined(BOOL_AS_INT)
# define false	0
# define true	1
#endif //BOOL_AS_INT
#define IS_TRUE(x) (true == (x))
#define IS_FALSE(x) (false == (x))

#define IS_ZERO(x) (0 == (x))
#define IS_NOT_ZERO(x) (0 != (x))
#define IS_NULL_PTR(p) (!(p))
#define IS_PTR(p) (p)

//-- Max/min
#define MAX(a,b) (((a) > (b)) ? (a) : (b))
#define MIN(a,b) (((a) > (b)) ? (b) : (a))
#define CLAMP(a,b,c) (MAX((a),MIN((b),(c))))

//-- Flags and bitfields
#define TEST_FLAG(bitField,flag)		(((bitField) & (flag)) == (flag))
#define SET_FLAG(bitField,flag)			((bitField) |= (flag))
#define CLEAR_FLAG(bitField,flag)		((bitField) &= ~(flag))

#define BIT(x)							(1 << (x))
#define TEST_BIT(bitField,bit)			(((bitField) & BIT(bit)) != 0)
#define SET_BIT(bitField,bit)			((bitField) |= BIT(bit))
#define CLEAR_BIT(bitField,bit)			((bitField) &= ~BIT(bit))

#define SAFE_DELETE(p)					if (IS_PTR(p)) { delete p; p = NULL; }
#define SAFE_DELETE_ARRAY(p)			if (IS_PTR(p)) { delete [] p; p = NULL; }


//----------------------------------------------------------//
// ENUMS
//----------------------------------------------------------//

//----------------------------------------------------------//
// STRUCTS
//----------------------------------------------------------//


#if defined(BOOL_AS_INT)
	typedef	int					bool;
#endif //BOOL_AS_INT

#if defined(WIN32)

	typedef float				f32;
	typedef double				f64;
	typedef int					s32;
	typedef unsigned int		u32;
	typedef char				s8;
	typedef unsigned char		u8;
	typedef short				s16;
	typedef unsigned short		u16;
	typedef long long			s64;
	typedef unsigned long long	u64;
	typedef unsigned int		bitfield;

#elif defined(LINUX)

	typedef float				f32;
	typedef double				f64;
	typedef int					s32;
	typedef unsigned int		u32;
	typedef char				s8;
	typedef unsigned char		u8;
	typedef short				s16;
	typedef unsigned short		u16;
	typedef long long			s64;
	typedef unsigned long long	u64;
	typedef unsigned int		bitfield;
	typedef size_t				uintptr_t;

#endif


//----------------------------------------------------------//
// CLASSES
//----------------------------------------------------------//

//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//

#endif //_TYPES_H_