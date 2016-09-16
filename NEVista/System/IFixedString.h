#ifndef _IFIXEDSTRING_H_
#define _IFIXEDSTRING_H_
#pragma once

//----------------------------------------------------------//
// IFIXEDSTRING.H
//----------------------------------------------------------//
//-- Description
// Interface to FixedString
//----------------------------------------------------------//


#include "Types.h"
#include "SysString.h"


//----------------------------------------------------------//
// DEFINES
//----------------------------------------------------------//

//----------------------------------------------------------//
// ENUMS
//----------------------------------------------------------//

//----------------------------------------------------------//
// STRUCTS
//----------------------------------------------------------//

//----------------------------------------------------------//
// CLASSES
//----------------------------------------------------------//


class IFixedString
{
	public:

		virtual ~IFixedString() {}
		
		virtual s8* Buffer(void) = 0;
		virtual const s8* ConstBuffer(void) const = 0;
		virtual size_t Length(void) const = 0;
		virtual size_t Size(void) const = 0;

		virtual SysString::Hash GenerateHash(void) const = 0;

		virtual void Clear(void) = 0;

		virtual bool IsEmpty(void) const = 0;

		virtual s8* Set(const s8* strIn) = 0;
		virtual s8* Set(const s8* strIn, size_t nStrInLength) = 0;
		virtual s8* Set(const IFixedString& in) = 0;

		virtual s8* Append(const s8* strIn) = 0;
		virtual s8* operator+=(const s8* strIn) = 0;
		virtual s8* Append(const IFixedString& in) = 0;
		virtual s8* operator+=(const IFixedString& in) = 0;

		virtual s32 Format(const s8 *strIn, ...) = 0;
};


//----------------------------------------------------------//
// EXTERNALS
//----------------------------------------------------------//

//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//

#endif //_IFIXEDSTRING_H_
