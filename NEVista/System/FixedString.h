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
class FixedString
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
		template <size_t S2> FixedString(FixedString<S2>& in)
		{
			Clear();
			Set(in);
		}

		~FixedString() {};
		
		s8* Buffer(void)
		{
			assert(IS_ZERO(m_strBuffer[S-1]));
			return m_strBuffer;
		}
		const s8* ConstBuffer(void) const
		{
			assert(IS_ZERO(m_strBuffer[S-1]));
			return m_strBuffer;
		}
		size_t Length(void)
		{
			return SysString::Strlen(m_strBuffer);
		}
		size_t Size(void) const
		{
			return S;
		}

		SysString::Hash GenerateHash(void) const
		{
			return SysString::GenerateHash(m_strBuffer);
		}

		void Clear(void)
		{
			m_strBuffer[0] = 0;
			m_strBuffer[S-1] = 0;
		}

		bool IsEmpty(void)
		{
			return SysString::IsEmpty(m_strBuffer);
		}

		s8* Set(const s8* strIn)
		{
			return SysString::Strcpy(m_strBuffer, S, strIn);
		}
		s8* Set(const s8* strIn, size_t nStrInLength)
		{
			return SysString::Strncpy(m_strBuffer, S, strIn, nStrInLength);
		}
		template <size_t S2> s8* Set(FixedString<S2>& in)
		{
			return Set(in.Buffer(), in.Length());
		}

		s8* Append(const s8* strIn)
		{
			return SysString::Strcat(m_strBuffer, S, strIn);
		}
		s8* operator+=(const s8* strIn)
		{
			return Append(strIn);
		}
		template <size_t S2> s8* Append(FixedString<S2>& in)
		{
			return Append(in.ConstBuffer());
		}
		template <size_t S2> s8* operator+=(FixedString<S2>& in)
		{
			return Append(in);
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
