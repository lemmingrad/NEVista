#ifndef _SIMPLE_BUFFER_SERIALIZER_H_
#define _SIMPLE_BUFFER_SERIALIZER_H_
#pragma once

//----------------------------------------------------------//
// SIMPLEBUFFERSERIALIZER.H
//----------------------------------------------------------//
//-- Description			
// Serializing class for serializing into a CSimpleBuffer.
//----------------------------------------------------------//


#include "Serializer.h"
#include "Types.h"
#include "ISimpleBuffer.h"


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


class CSimpleBufferSerializer : public CSerializer
{
	public:

		struct Error 
		{
			enum Enum
			{
				SizeMismatch = -7, 
				MoveFailed = -6,
				CopyFailed = -5,
				BadParameter = -4,
				WouldUnderflow = -3,
				WouldOverflow = -2,
				Fail = -1,
				Ok = 0
			};
		};

		CSimpleBufferSerializer(ISimpleBuffer& Buffer, bool bCompress = false, bool bIncludeFourCCs = false);
		virtual ~CSimpleBufferSerializer();

		// ISerializer
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
		virtual size_t			SerializeString(std::string& strng, u32 nFourCC = 'sstr');
		virtual size_t			SerializeFixedString(IFixedString& fixedString, u32 nFourCC = 'fstr'); 
		// ~ISerializer

		Error::Enum				GetError() const;

	private:
	
		template <class TType>
		size_t SerializeSignedCompressed(TType value, u32 nFourCC)
		{
			m_eError = Error::Ok;

			UBAu64 tempStore;
			u8* pByte = tempStore.m_Bytes;
			size_t nCount = 0;
			bool bKeepGoing = true;

			while (IS_TRUE(bKeepGoing))
			{
				bool bSignBitSet = IS_NOT_ZERO(value & 0x40);
				*pByte = u8(value & 0x7f);
				value >>= 7;
				bKeepGoing = (IS_FALSE(bSignBitSet) && IS_NOT_ZERO(value)) || (IS_TRUE(bSignBitSet) && (value != TType(-1)));
				*pByte |= (bKeepGoing) ? (1 << 7) : 0;
				++pByte;
				++nCount;
			}

			return SerializeBytes(tempStore.m_Bytes, nCount, nFourCC);
		}

		template <class TType> 
		size_t SerializeUnsignedCompressed(TType value, u32 nFourCC)
		{
			m_eError = Error::Ok;

			UBAu64 tempStore;
			u8* pByte = tempStore.m_Bytes;
			size_t nCount = 0;
			bool bKeepGoing = true;

			while (IS_TRUE(bKeepGoing))
			{
				*pByte = u8(value & 0x7f);
				value >>= 7;
				bKeepGoing = (value > 0);
				*pByte |= (bKeepGoing) ? (1 << 7) : 0;
				++pByte;
				++nCount;
			}

			return SerializeBytes(tempStore.m_Bytes, nCount, nFourCC);
		}

		Error::Enum				ConvertError(ISimpleBuffer::Error::Enum e);

		ISimpleBuffer&			m_Buffer;
		Error::Enum				m_eError;
		bool					m_bCompress;
		bool					m_bIncludeFourCCs;
};


//----------------------------------------------------------//
// EXTERNALS
//----------------------------------------------------------//

//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//

#endif //_SIMPLE_BUFFER_SERIALIZER_H_