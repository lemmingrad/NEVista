//----------------------------------------------------------//
// SYSSTRING.CPP
//----------------------------------------------------------//
//-- Description
// Wrapper for string functions to hide implementation of 
// safe and non-safe string handlers.
//----------------------------------------------------------//


#include <stdarg.h>
#include <string.h>
#include <string>

#include "Types.h"
#include "SysMemory.h"
#include "SysString.h"


//----------------------------------------------------------//
// DEFINES
//----------------------------------------------------------//


//-- Define this to use safe strings with bounds checking as seen in VC8 and VC9
#define SYSSTRING_USES_SAFE_STRINGS


//----------------------------------------------------------//
// GLOBALS
//----------------------------------------------------------//


//-- dictionary 0 is always default recognised base64 dictionary - [A-Z][a-z][0-9]+/
const std::string String::KeyDictionary[NUM_KEY_DICTIONARIES] = { 
	"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/",
	"AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz0123456789+/",
	"A1B2C3D4E5F6G7H8I9J0KLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz+/",
	"0aA1bB2cC3dD4eE5fF6gG7hH8iI9jJkKlLmMnNoOpPqQrRsStTuUvVwWxXyYzZ+/",
};


//----------------------------------------------------------//
// String::Strlen
//----------------------------------------------------------//
//-- Description
// Get the length of s string
//----------------------------------------------------------//
size_t String::Strlen(const s8* strSrc)
{
	return strlen(strSrc);
}


//----------------------------------------------------------//
// String::Strcpy
//----------------------------------------------------------//
//-- Description
// Copy one string buffer to another
//----------------------------------------------------------//
s8* String::Strcpy(s8* strDest, size_t nDestSize, const s8* strSrc)
{
#if defined(SYSSTRING_USES_SAFE_STRINGS)

   //-- Use the new safe strcpy
	errno_t nError = strcpy_s(strDest, nDestSize, strSrc);
	if (nError == 0)
	{
		return strDest;
	}
	else
	{
		return NULL;
	}

#else

   //-- Emulate the safe string copy
	if (!strDest || (0 == nDestSize) || !strSrc)
	{
		//-- Be consistent with the behavior of strcpy_s.
		return NULL;
	}

	size_t nSrcLen = Strlen(strSrc);

	return Strncpy(strDest, nDestSize, strSrc, nSrcLen);

#endif
}


//----------------------------------------------------------//
// String::Strncpy
//----------------------------------------------------------//
//-- Description
// Copy one string buffer to another, up to a maximum
// number of characters.
//----------------------------------------------------------//
s8* String::Strncpy(s8* strDest, size_t nDestSize, const s8* strSrc, size_t nSrcSize)
{
#if defined(SYSSTRING_USES_SAFE_STRINGS)
    
	//-- Use the new safe strncpy
	errno_t nError = strncpy_s(strDest, nDestSize, strSrc, nSrcSize);
	if (nError == 0)
	{
			return strDest;
	}
	else
	{
			return NULL;
	}

#else

    //-- Emulate the safe string copy
	if (!strDest || (nDestSize == 0) || !strSrc || nSrcSize == 0)
	{
		//-- Be consistent with the behavior of strncpy_s.
		return NULL;
	}

	if (nSrcSize > (nDestSize - 1))
	{
		//-- The source string is too large to copy to the destination.  To
		//-- be consistent with strncpy_s, return null as an indication that
		//-- the copy failed.
		return NULL;
	}

	strncpy(strDest, strSrc, nDestSize);
	strDest[nDestSize - 1] = 0;

	return strDest;

#endif
}


//----------------------------------------------------------//
// String::Strcat
//----------------------------------------------------------//
//-- Description
// Append one string to another
//----------------------------------------------------------//
s8* String::Strcat(s8* strDest, size_t nDestSize, const s8* strSrc)
{
#if defined(SYSSTRING_USES_SAFE_STRINGS)

	errno_t nError = strcat_s(strDest, nDestSize, strSrc);
	if (nError == 0)
	{
		return strDest;
	}
	else
	{
		return NULL;
	}

#else

    //-- Emulate the safe string copy
	if (!strDest || (nDestSize == 0) || !strSrc)
	{
		//-- Be consistent with the behavior of strcat_s.
		return NULL;
	}

	size_t nSrcLen = Strlen(strSrc);
	size_t nDestLen = Strlen(strDest);
	size_t nTotalLen = nSrcLen + nDestLen;
    
	if (nTotalLen > (nDestSize -1))
	{
		//-- The source string is too large to append to the destination.  To
		//-- be consistent with strcat_s, return null as an indication that
		//-- the concatenation failed.
		return NULL;
	}

	strncat(strDest, strSrc, nSrcLen);
	strDest[nTotalLen] = 0;

	return strDest;

#endif
}


//----------------------------------------------------------//
// String::Strcmp
//----------------------------------------------------------//
//-- Description
// Compares the contents of a string with another string
// and returns 0 if they match exactly
//----------------------------------------------------------//
s32 String::Strcmp(const s8* strOne, const s8* strTwo)
{
	return (s32)strcmp(strOne, strTwo);
}


//----------------------------------------------------------//
// String::Strncmp
//----------------------------------------------------------//
//-- Description
// Compares the contents of a string with another string, up
// to a maximum number of characters, and returns 0 if they 
// match exactly
//----------------------------------------------------------//
s32 String::Strncmp(const s8* strOne, const s8* strTwo, const u32 nCount)
{
	return (s32)strncmp(strOne, strTwo, nCount);
}


//----------------------------------------------------------//
// String::Stricmp
//----------------------------------------------------------//
//-- Description
// Compares the contents of a string with another string
// and returns 0 if they match exactly. CASE INDEPENDENT
//----------------------------------------------------------//
s32 String::Stricmp(const s8* strOne, const s8* strTwo)
{
	return (s32)_stricmp(strOne, strTwo);
}


//----------------------------------------------------------//
// String::Strnicmp
//----------------------------------------------------------//
//-- Description
// Compares the contents of a string with another string, up
// to a maximum number of characters, and returns 0 if they 
// match exactly. CASE INDEPENDENT
//----------------------------------------------------------//
s32 String::Strnicmp(const s8* strOne, const s8* strTwo, size_t nCount)
{
	return (s32)_strnicmp(strOne, strTwo, nCount);
}


//----------------------------------------------------------//
// String::Strtok
//----------------------------------------------------------//
//-- Description
// Find the next token in a string.
//----------------------------------------------------------//
s8* String::Strtok(s8* strToken, const s8* strDelimiters, s8*& strNextToken)
{
#if defined(SYSSTRING_USES_SAFE_STRINGS)

	return strtok_s(strToken, strDelimiters, &strNextToken);

#else

	return strtok(strToken, strDelimiters);

#endif
}


//----------------------------------------------------------//
// String::Strstr
//----------------------------------------------------------//
//-- Description
// Find the first occurance of a substring inside a larger
// string.
//----------------------------------------------------------//
const s8* String::Strstr(const s8* strSrc, const s8* strFind)
{
	return strstr(strSrc, strFind);
}


//----------------------------------------------------------//
// String::Strstr
//----------------------------------------------------------//
//-- Description
// Converts a string to uppercase in place
//----------------------------------------------------------//
s8* String::Strupr(s8* strSrc, size_t nSrcSize)
{
#if defined(SYSSTRING_USES_SAFE_STRINGS)

	errno_t nError = _strupr_s(strSrc, nSrcSize);
	if (nError == 0)
	{
		return strSrc;
	}
	else
	{
		return NULL;
	}

#else

	return _strupr(strSrc);

#endif
}
		

//----------------------------------------------------------//
// String::Strlwr
//----------------------------------------------------------//
//-- Description
// Converts a string to lowercase in place
//----------------------------------------------------------//
s8* String::Strlwr(s8* strSrc, size_t nSrcSize)
{
#if defined(SYSSTRING_USES_SAFE_STRINGS)

	errno_t nError = _strlwr_s(strSrc, nSrcSize);
	if (nError == 0)
	{
		return strSrc;
	}
	else
	{
		return NULL;
	}

#else

	return _strlwr(strSrc);

#endif
}


//----------------------------------------------------------//
// String::Sprintf
//----------------------------------------------------------//
//-- Description
// Writes a formatted multi-parameter string
//----------------------------------------------------------//
s32 String::Sprintf(s8* strDest, size_t nDestSize, const s8* strFormating, ...)
{
	s32 nSymbolsConverted = -1;

	if (!strDest || (nDestSize == 0) || !strFormating)
	{
		return nSymbolsConverted;
	}

	va_list ArgList;
	va_start(ArgList, strFormating);
	nSymbolsConverted = Vsprintf(strDest, nDestSize, strFormating, ArgList);
	va_end(ArgList);

	return nSymbolsConverted;
}


//----------------------------------------------------------//
// String::Vsprintf
//----------------------------------------------------------//
//-- Description
// Writes a formatted multi-parameter string using existing
// va_list data.
//----------------------------------------------------------//
s32 String::Vsprintf(s8* strDest, size_t nDestSize, const s8* strFormating, va_list ArgList)
{
   s32 nSymbolsConverted = -1;
	
	if (!strDest || (nDestSize == 0) || !strFormating)
  {
      return nSymbolsConverted;
  }

#if defined(SYSSTRING_USES_SAFE_STRINGS)

  nSymbolsConverted = vsprintf_s(strDest, nDestSize, strFormating, ArgList);

#else

	nSymbolsConverted = vsprintf(strDest, strFormating, ArgList);

#endif

  return nSymbolsConverted;
}


//----------------------------------------------------------//
// String::Atof
//----------------------------------------------------------//
//-- Description
// Converts a string representation of a number into an
// actual number
//----------------------------------------------------------//
f32 String::Atof(const s8* strBuffer)
{
	return (f32)atof(strBuffer);
}


//----------------------------------------------------------//
// String::Atoi
//----------------------------------------------------------//
//-- Description
// Converts a string representation of a number into an
// actual number
//----------------------------------------------------------//
s32 String::Atoi(const s8* strBuffer)
{
	return (s32)atoi(strBuffer);
}


//----------------------------------------------------------//
// String::GenerateHash
//----------------------------------------------------------//
//-- Description
// Hashes a string into a unique ID.
// Theoretically, there's 2 billion or so unique numbers,
// so we can probably treat any Hash as unique without 
// worrying about a conflict.
//----------------------------------------------------------//
String::Hash String::GenerateHash(const s8* strBuffer)
{
	if ((strBuffer == NULL) || (strBuffer[0] == '\0'))
	{
		//-- 0 length string
		return INVALID_HASH;
	}

	size_t strLength = Strlen(strBuffer) * 8;

	Hash nReturn = 1;
	u8 f0, f1, f2, f3, f4;

	u8 nMask = 0x01;

	u32 nIndex = 0;
	s8 nChar = strBuffer[nIndex];
	if ( (nChar >= 97) && (nChar <= 122) )
	{
		nChar -= 32;
	}

	for (u32 i = 0; i < strLength; ++i)
	{
		if (nReturn & 0x80000000) f0 = 1; else f0 = 0;
		if (nReturn & 0x00200000) f1 = 1; else f1 = 0;
		if (nReturn & 0x00000002) f2 = 1; else f2 = 0;
		if (nReturn & 0x00000001) f3 = 1; else f3 = 0;
		if (nChar & nMask) f4 = 1; else f4 = 0;

		nReturn = nReturn << 1;
		if (f0 ^ f1 ^ f2 ^ f3 ^ f4) nReturn |= 0x01;

		nMask = (u8)(nMask << 1);
		if (!nMask)
		{
			nMask = 0x01;

			++nIndex;
			nChar = strBuffer[nIndex];
			if ((nChar >= 97) && (nChar <= 122))
			{
				nChar -= 32;
			}
		}
	}

	return nReturn;
}


//----------------------------------------------------------//
// String::GenerateKey
//----------------------------------------------------------//
//-- Description
// Create a string encryption key from 2 base parts.
// Note that GenerateKey(0,0) will produce a key that
// uses the default base64 dictionary of [A-Z][a-z][0-9]+/. 
//----------------------------------------------------------//
String::Key String::GenerateKey(u8 part1, u8 part2)
{
	return Key(((part1 % NUM_KEY_DICTIONARIES) << 8) | (part2 % 64));
}


//----------------------------------------------------------//
// String::Base64Encode
//----------------------------------------------------------//
//-- Description
// Convert a byte buffer into a base64 string using standard 
// convention [A-Z][a-z][0-9]+/ and padded with '='.
//----------------------------------------------------------//
size_t String::Base64Encode(s8* strBuffer, size_t strBufferSize, const void* pDataBuffer, size_t dataSize)
{
	return KeyEncode(strBuffer, strBufferSize, pDataBuffer, dataSize, GenerateKey(0, 0));
}


//----------------------------------------------------------//
// String::Base64Decode
//----------------------------------------------------------//
//-- Description
// Convert a standard format base64 string into a byte buffer.
//----------------------------------------------------------//
size_t String::Base64Decode(void* pDataBuffer, size_t dataBufferSize, const s8* strBuffer, size_t strLength)
{
	return KeyDecode(pDataBuffer, dataBufferSize, strBuffer, strLength, GenerateKey(0, 0));
}


//----------------------------------------------------------//
// String::KeyEncode
//----------------------------------------------------------//
//-- Description
// Convert a byte buffer into a base64 string using
// non-standard dictionaries determined by an encryption
// key. Note that GenerateKey(0,0) will produce a key that
// uses the default base64 dictionary of [A-Z][a-z][0-9]+/. 
// Results are always padded with '=' up to blocks of
// 4 characters.
//----------------------------------------------------------//
size_t String::KeyEncode(s8* strBuffer, size_t strBufferSize, const void* pDataBuffer, size_t dataSize, String::Key key)
{
	if (!pDataBuffer || (dataSize == 0) || !strBuffer || (strBufferSize < 1))
	{
		return 0;
	}

	size_t base64Len = ((dataSize + 2) / 3) * 4;

	if (strBufferSize < (base64Len + 1))
	{
		//-- Output buffer not big enough!
		return 0;
	}

	u8 keyrotation = key & 0xFF;
	u8 keydictionary = (key >> 8) & 0xFF;
	const std::string& dictionaryRef = KeyDictionary[keydictionary];
	std::string dictionary;
	dictionary.append(&(dictionaryRef.c_str()[keyrotation]));
	dictionary.append(dictionaryRef.c_str());

	Memory::Memset(strBuffer, '=', base64Len);

	const u8* pDataBytes = (const u8*)pDataBuffer;
	size_t byteCount = 0;

	for (size_t i = 0; i < base64Len; i += 4)
	{
		size_t remainder = dataSize - byteCount;
		u8 temp[3];
		temp[0] = (byteCount < dataSize) ? pDataBytes[byteCount++] : 0;
		temp[1] = (byteCount < dataSize) ? pDataBytes[byteCount++] : 0;
		temp[2] = (byteCount < dataSize) ? pDataBytes[byteCount++] : 0;

		u8 val = 0;

		val = temp[0] >> 2;
		strBuffer[i] = dictionary[val];

		val = (temp[0] & 0x03) << 4;
		val |= temp[1] >> 4;
		strBuffer[i+1] = dictionary[val];

		if (remainder >= 2)
		{
			val = (temp[1] & 0x0f) << 2;
			val |= temp[2] >> 6;
			strBuffer[i+2] = dictionary[val];

			if (remainder >= 3)
			{
				val = temp[2] & 0x3f;
				strBuffer[i+3] = dictionary[val];
			}
		}
	}

	strBuffer[base64Len] = '\0';

	return base64Len;
}


//----------------------------------------------------------//
// String::KeyDecode
//----------------------------------------------------------//
//-- Description
// Convert a non-standard base64 string into a byte buffer.
//----------------------------------------------------------//
size_t String::KeyDecode(void* pDataBuffer, size_t dataBufferSize, const s8* strBuffer, size_t strLength, String::Key key)
{
	if (!strBuffer || (strLength == 0) || !pDataBuffer || (dataBufferSize == 0) || ((strLength % 4) != 0))
	{
		return 0;
	}

	size_t bufLength = (strLength / 4) * 3;
	size_t bufShrunk = bufLength;

	if (strBuffer[strLength-1] == '=')
	{
		--bufShrunk;
		if (strBuffer[strLength-2] == '=')
		{
			--bufShrunk;
		}
	}

	if (dataBufferSize < bufShrunk)
	{
		//-- data buffer not big enough
		return 0;
	}

	u8 keyrotation = key & 0xFF;
	u8 keydictionary = (key >> 8) & 0xFF;
	const std::string& dictionaryRef = KeyDictionary[keydictionary];
	std::string dictionary;
	dictionary.append(&(dictionaryRef.c_str()[keyrotation]));
	dictionary.append(dictionaryRef.c_str());

	Memory::Memclear(pDataBuffer, bufShrunk);

	u8* pDataBytes = (u8*)pDataBuffer;
	size_t charIndex = 0;

	for (size_t i = 0; i < bufLength; i += 3)
	{
		s8 temp[4];
		temp[0] = (charIndex < strLength) ? dictionary.find(strBuffer[charIndex++]) : -1;
		temp[1] = (charIndex < strLength) ? dictionary.find(strBuffer[charIndex++]) : -1;
		temp[2] = (charIndex < strLength) ? dictionary.find(strBuffer[charIndex++]) : -1;
		temp[3] = (charIndex < strLength) ? dictionary.find(strBuffer[charIndex++]) : -1;

		s8 c = 0;

		c = (temp[0] << 2);
		c |= (temp[1] & 0x30) >> 4;
		pDataBytes[i] = c;

		if (temp[2] != -1)
		{
			c = (temp[1] & 0x0f) << 4;
			c |= (temp[2] & 0x3c) >> 2;
			pDataBytes[i+1] = c;

			if (temp[3] != -1)
			{
				c = (temp[2] & 0x03) << 6;
				c |= temp[3];
				pDataBytes[i+2] = c;
			}
		}
		else
		{
			break;
		}
	}

	return bufShrunk;
}

//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//
