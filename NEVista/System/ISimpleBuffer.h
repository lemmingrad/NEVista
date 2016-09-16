#ifndef _INTERFACE_SIMPLE_BUFFER_H_
#define _INTERFACE_SIMPLE_BUFFER_H_
#pragma once

//----------------------------------------------------------//
// ISIMPLEBUFFER.H
//----------------------------------------------------------//
//-- Description
// Interface to CSimpleBuffer
//----------------------------------------------------------//


#include "Types.h"
#include "SysMemory.h"


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


class ISimpleBuffer
{
	public:

		struct Error
		{
			enum Enum
			{
				MoveFailed = -6,
				CopyFailed = -5,
				BadParameter = -4,
				WouldUnderflow = -3,
				WouldOverflow = -2,
				Fail = -1,
				Ok = 0
			};
		}; 
		
		virtual ~ISimpleBuffer() {}
		
		virtual u8* Buffer(void) = 0;
		virtual const u8* ConstBuffer(void) const = 0;

		virtual size_t Size(void) const = 0;
		virtual size_t UsedSize(void) const = 0;
		virtual size_t UnusedSize(void) const = 0;

		virtual Error::Enum GetError() const = 0;

		virtual void Clear(void) = 0;

		virtual bool IsEmpty(void) = 0;

		virtual u8* InsHead(const u8* pIn, size_t nInSize) = 0;
		virtual u8* InsTail(const u8* pIn, size_t nInSize) = 0;

		virtual u8* StripHead(u8* pOut, size_t nOutSize) = 0;
		virtual u8* StripTail(u8* pOut, size_t nOutSize) = 0;
};


//----------------------------------------------------------//
// EXTERNALS
//----------------------------------------------------------//

//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//

#endif //_INTERFACE_SIMPLE_BUFFER_H_
