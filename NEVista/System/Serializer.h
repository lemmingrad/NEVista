#ifndef _SERIALIZER_H_
#define _SERIALIZER_H_
#pragma once

//----------------------------------------------------------//
// SERIALIZER.H
//----------------------------------------------------------//
//-- Description			
// Base implementation of a serializer class.
//----------------------------------------------------------//


#include "ISerializer.h"
#include "Types.h"


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


class CSerializer : public ISerializer
{
	public:

		CSerializer(Mode::Enum eMode) : m_eMode(eMode) {}
		virtual ~CSerializer() {}

		// ISerializer
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
		virtual Mode::Enum		GetMode(void) const { return m_eMode; }
		// ~ISerializer

	protected:

		union UBAf32
		{
			f32		m_fValue;
			u8		m_Bytes[sizeof(f32)];
		};
		union UBAf64
		{
			f64		m_fValue;
			u8		m_Bytes[sizeof(f64)];
		};
		union UBAs32
		{
			s32		m_nValue;
			u8		m_Bytes[sizeof(s32)];
		};
		union UBAs16
		{
			s16		m_nValue;
			u8		m_Bytes[sizeof(s16)];
		};
		union UBAs64
		{
			s64		m_nValue;
			u8		m_Bytes[sizeof(s64)];
		};
		union UBAu32
		{
			u32		m_nValue;
			u8		m_Bytes[sizeof(u32)];
		};
		union UBAu16
		{
			u16		m_nValue;
			u8		m_Bytes[sizeof(u16)];
		};
		union UBAu64
		{
			u64		m_nValue;
			u8		m_Bytes[sizeof(u64)];
		};
		union UBAbitfield
		{
			bitfield m_nValue;
			u8		m_Bytes[sizeof(bitfield)];
		};
		union UBAFourCC
		{
			u32		m_nValue;
			u8		m_Bytes[sizeof(u32)];
		};

		Mode::Enum				m_eMode;
};


//----------------------------------------------------------//
// EXTERNALS
//----------------------------------------------------------//

//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//

#endif //_SERIALIZER_H_