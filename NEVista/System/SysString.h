#ifndef _SYS_STRING_H_
#define _SYS_STRING_H_
#pragma once

//----------------------------------------------------------//
// SYSSTRING.H
//----------------------------------------------------------//
//-- Description
// Wrapper for string functions to hide implementation of 
// safe and non-safe string handlers.
//----------------------------------------------------------//


#include "Types.h"
#include <string>


//----------------------------------------------------------//
// DEFINES
//----------------------------------------------------------//


#define NUM_KEY_DICTIONARIES				4
#define NUM_KEY_ROTATIONS					64


//----------------------------------------------------------//
// ENUMS
//----------------------------------------------------------//

//----------------------------------------------------------//
// STRUCTS
//----------------------------------------------------------//

//----------------------------------------------------------//
// CLASSES
//----------------------------------------------------------//


class SysString
{
	public:

		typedef u32			Hash;
		typedef u16			Key;
		static const Hash	INVALID_HASH = (Hash)0;

		static u32			Strlen(const s8* strSrc);

		static s8*			Strcpy(s8* strDest, size_t nDestSize, const s8* strSrc);
		static s8*			Strncpy(s8* strDest, size_t nDestSize, const s8* strSrc, size_t nSrcSize);

		static s8*			Strcat(s8* strDest, size_t nDestSize, const s8* strSrc);
		
		static s32			Strcmp(const s8* srcOne, const s8* srcTwo);
		static s32			Stricmp(const s8* srcOne, const s8* srcTwo);
		static s32			Strncmp(const s8* srcOne, const s8* srcTwo, size_t nCount);
		static s32			Strnicmp(const s8* srcOne, const s8* srcTwo, size_t nCount);
		
		static s8*			Strtok(s8* strToken, const s8* strDelimiters, s8*& strNextToken);

		static const s8*	Strstr(const s8* strSrc, const s8* strFind);

		static s8*			Strupr(s8* strSrc, size_t nSrcSize);
		static s8*			Strlwr(s8* strSrc, size_t nSrcSize);

		static s32			Sprintf(s8* strDest, size_t nDestSize, const s8* strFormating, ...);
		static s32			Vsprintf(s8* strDest, size_t nDestSize, const s8* strFormating, va_list ArgList);

		static f32			Atof(const s8* strBuffer);
		static s32			Atoi(const s8* strBuffer);

		static Hash			GenerateHash(const s8* strBuffer);

		static Key			GenerateKey(u8 part1, u8 part2);

		static size_t		Base64Encode(s8* strBuffer, size_t strBufferSize, const void* pDataBuffer, size_t dataSize);
		static size_t 		Base64Decode(void* pDataBuffer, size_t strDataSize, const s8* strBuffer, size_t strBufferSize);

		static size_t		KeyEncode(s8* strBuffer, size_t strBufferSize, const void* pDataBuffer, size_t dataSize, Key key);
		static size_t		KeyDecode(void* pDataBuffer, size_t dataBufferSize, const s8* strBuffer, size_t strLength, SysString::Key key);

		static s8*			FourCC(s8* strDest, size_t nDestSize, u32 nFourCC);

		static bool			IsEmpty(const s8* strBuffer);

	private:

		static const std::string KeyDictionary[NUM_KEY_DICTIONARIES];

		SysString();
		~SysString();
};

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
			return m_strBuffer;
		}
		const s8* ConstBuffer(void) const
		{
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
			return Append(in.Buffer());
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

#endif //_SYS_STRING_H_
