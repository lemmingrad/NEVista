#ifndef _ISERIALIZER_H_
#define _ISERIALIZER_H_
#pragma once

//----------------------------------------------------------//
// ISERIALIZER.H
//----------------------------------------------------------//
//-- Description			
// Interface defining a serializer class.
// Yes, I know it breaks true interface design by including
// an enum and a non-abstract class.
//----------------------------------------------------------//


#include "Types.h"
#include <string>


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


class IFixedString;


class ISerializer
{
	public:

		struct Mode
		{
			enum Enum
			{
				Unknown = 0,
				Serializing,
				Deserializing
			};
		};

		virtual ~ISerializer() {}

		virtual	size_t			SerializeF32(f32& fValue, u32 nFourCC = 'f32 ') = 0; 
		virtual	size_t			SerializeF64(f64& fValue, u32 nFourCC = 'f64 ') = 0; 
		virtual	size_t			SerializeS32(s32& nValue, u32 nFourCC = 's32 ') = 0; 
		virtual	size_t			SerializeU32(u32& nValue, u32 nFourCC = 'u32 ') = 0; 
		virtual	size_t			SerializeS8(s8& nValue, u32 nFourCC = 's8  ') = 0; 
		virtual	size_t			SerializeU8(u8& nValue, u32 nFourCC = 'u8  ') = 0; 
		virtual	size_t			SerializeS16(s16& nValue, u32 nFourCC = 's16 ') = 0; 
		virtual	size_t			SerializeU16(u16& nValue, u32 nFourCC = 'u16 ') = 0; 
		virtual	size_t			SerializeS64(s64& nValue, u32 nFourCC = 's64 ') = 0; 
		virtual	size_t			SerializeU64(u64& nValue, u32 nFourCC = 'u64 ') = 0; 
		virtual	size_t			SerializeBitfield(bitfield& nFlags, u32 nFourCC = 'bits') = 0; 
		virtual size_t			SerializeBytes(u8* pData, size_t nDataSize, u32 nFourCC = 'data') = 0;
		virtual size_t			SerializeBool(bool& bValue, u32 nFourCC = 'bool') = 0;
		virtual size_t			SerializeString(std::string& strng, u32 nFourCC = 'sstr') = 0;
		virtual size_t			SerializeFixedString(IFixedString& fixedString, u32 nFourCC = 'fstr') = 0;

		virtual Mode::Enum		GetMode(void) const = 0;
};


//----------------------------------------------------------//
// EXTERNALS
//----------------------------------------------------------//

//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//

#endif //_SERIALIZER_H_