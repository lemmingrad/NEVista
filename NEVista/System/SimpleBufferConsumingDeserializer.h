#ifndef _BYTE_DESERIALIZER_H_
#define _BYTE_DESERIALIZER_H_
#pragma once

//----------------------------------------------------------//
// BYTEDESERIALIZER.H
//----------------------------------------------------------//
//-- Description			
// Basic deserializing class for byte deserializing.
//----------------------------------------------------------//


#include "Types.h"
#include "ISerializer.h"


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


class CSimpleBuffer;


class CByteDeserializer
{
public:

	CByteDeserializer(CSimpleBuffer& Buffer) : ISerializer(ISerializer::Mode::Deserializing) {}
	virtual ~CByteDeserializer() {}

	virtual	size_t			SerializeF32(f32& fValue, u32 nFourCC = 'f32 ');
	virtual	size_t			SerializeF64(f64& fValue, u32 nFourCC = 'f64 ');
	virtual	size_t			SerializeS32(s32& nValue, u32 nFourCC = 's32 ');
	virtual	size_t			SerializeU32(u32& nValue, u32 nFourCC = 'u32 ');
	virtual	size_t			SerializeS8(s8& nValue, u32 nFourCC = 's8  ');
	virtual	size_t			SerializeU8(u8& nValue, u32 nFourCC = 'u8  ');
	virtual	size_t			SerializeS16(s16& nValue, u32 nFourCC = 's16 ');
	virtual	size_t			SerializeU16(u16& nValue, u32 nFourCC = 'u16 ');
	virtual	size_t			SerializeS64(s64& nValue, u32 nFourCC = 's64 ');
	virtual	size_t			SerializeU64(u64& nValue, u32 nFourCC = 'u64 ');
	virtual	size_t			SerializeBitfield(bitfield& nFlags, u32 nFourCC = 'bits');
	virtual size_t			SerializeBytes(u8* pData, size_t nDataSize, u32 nFourCC = 'data');
	virtual size_t			SerializeBool(bool& bValue, u32 nFourCC = 'bool');
	virtual size_t			SerializeFixedString(FixedString& fixedString, u32 nFourCC = "fstr");
};


//----------------------------------------------------------//
// EXTERNALS
//----------------------------------------------------------//

//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//

#endif //_BYTE_DESERIALIZER_H_