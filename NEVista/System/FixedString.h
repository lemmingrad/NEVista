#ifndef _FIXEDSTRING_H_
#define _FIXEDSTRING_H_
#pragma once

//----------------------------------------------------------//
// FIXEDSTRING.H
//----------------------------------------------------------//
//-- Description
// String wrapper for a fixed-size string buffer object
//----------------------------------------------------------//


#include "Types.h"
#include "SysString.h"
#include "IFixedString.h"


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


template <size_t S>
class FixedString : public IFixedString
{
	public:

		FixedString()
		{
			Clear();
		}
		FixedString(const s8* strIn)
		{
			Clear();
			Set(strIn);
		}
		FixedString(const IFixedString& in)
		{
			Clear();
			Set(in);
		}

		virtual ~FixedString() {}
		
		virtual s8* Buffer(void)
		{
			assert(IS_ZERO(m_strBuffer[S-1]));
			return m_strBuffer;
		}
		virtual const s8* ConstBuffer(void) const
		{
			assert(IS_ZERO(m_strBuffer[S-1]));
			return m_strBuffer;
		}
		virtual size_t Length(void) const
		{
			return SysString::Strlen(m_strBuffer);
		}
		virtual size_t Size(void) const
		{
			return S;
		}

		virtual SysString::Hash GenerateHash(void) const
		{
			return SysString::GenerateHash(m_strBuffer);
		}

		virtual void Clear(void)
		{
			m_strBuffer[0] = 0;
			m_strBuffer[S-1] = 0;
		}

		virtual bool IsEmpty(void) const
		{
			return IS_TRUE(SysString::IsEmpty(m_strBuffer));
		}

		virtual s8* Set(const s8* strIn)
		{
			s8* pRet = SysString::Strcpy(m_strBuffer, S, strIn);
			m_strBuffer[S-1] = 0;
			return pRet;
		}
		virtual s8* Set(const s8* strIn, size_t nStrInLength)
		{
			s8* pRet = SysString::Strncpy(m_strBuffer, S, strIn, nStrInLength);
			m_strBuffer[S-1] = 0;
			return pRet;
		}
		virtual s8* Set(const IFixedString& in)
		{
			return Set(in.ConstBuffer(), in.Length());
		}

		virtual s8* Append(const s8* strIn)
		{
			return SysString::Strcat(m_strBuffer, S, strIn);
		}
		virtual s8* operator+=(const s8* strIn)
		{
			return Append(strIn);
		}
		virtual s8* Append(const IFixedString& in)
		{
			return Append(in.ConstBuffer());
		}
		virtual s8* operator+=(const IFixedString& in)
		{
			return Append(in);
		}

		virtual s32 Format(const s8 *strIn, ...)
		{
			va_list ArgList;
			va_start(ArgList, strIn);
			s32 nLen = SysString::Vsprintf(m_strBuffer, S, strIn, ArgList);
			va_end(ArgList);
			return nLen;
		}

	private:

		s8		m_strBuffer[S];
};


//----------------------------------------------------------//
// EXTERNALS
//----------------------------------------------------------//

//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//

#endif //_FIXEDSTRING_H_
