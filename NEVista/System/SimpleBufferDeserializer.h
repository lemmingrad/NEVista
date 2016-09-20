#ifndef _SIMPLE_BUFFER_DESERIALIZER_H_
#define _SIMPLE_BUFFER_DESERIALIZER_H_
#pragma once

//----------------------------------------------------------//
// SIMPLEBUFFERDESERIALIZER.H
//----------------------------------------------------------//
//-- Description			
// Serializing class for deserializing from a CSimpleBuffer.
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


class CSimpleBufferDeserializer : public CSerializer
{
	public:

		struct Error
		{
			enum Enum
			{
				SizeMismatch = -8,
				FourCCMismatch = -7,
				MoveFailed = -6,
				CopyFailed = -5,
				BadParameter = -4,
				WouldUnderflow = -3,
				WouldOverflow = -2,
				Fail = -1,
				Ok = 0
			};
		};

		CSimpleBufferDeserializer(ISimpleBuffer& Buffer, bool bDecompress = false, bool bExpectFourCCs = false);
		virtual ~CSimpleBufferDeserializer();

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
		size_t SerializeSignedDecompressed(TType& value, u32 nFourCC)
		{
			u8 byte;
			u32 nShift = 0;
			size_t nBytesRead = 0;

			value = 0;

			//-- Always read at least one byte the normal way.
			//-- (to handle the fourCC)
			nBytesRead += SerializeBytes(&byte, 1, nFourCC);
			if (nBytesRead > 0)
			{
				value |= TType(byte & 0x7f) << nShift;
				nShift += 7;
	
				while (byte >= 0x80)
				{
					if (IS_PTR(m_Buffer.StripHead(&byte, 1)))
					{
						value |= TType(byte & 0x7f) << nShift;
						nShift += 7;
						++nBytesRead;
					}
					else
					{
						//-- Error in read. Manually set m_eError.
						m_eError = ConvertError(m_Buffer.GetError());
						byte = 0;
						nBytesRead = 0;
					}

					if (nBytesRead > (sizeof(u32) + sizeof(u64)))
					{
						//-- Safety test - we can never read more than 12 bytes. (fourcc + u64)
						//-- With LEB128 it should be less, but if we ever read more than 12,
						//-- we know something is wrong and we can stop.
						m_eError = Error::SizeMismatch;
						byte = 0;
						nBytesRead = 0;
					}
				} 

				//-- Sign extend negative numbers
				if ((nShift < 8 * sizeof(TType)) && (byte & 0x40))
				{
					value |= TType(-1) << nShift;
				}
			}
			else
			{
				//-- Error in read. m_eError already set by SerializeBytes().
			}

			return nBytesRead;
		}
		
		template <class TType>
		size_t SerializeUnsignedDecompressed(TType& value, u32 nFourCC)
		{
			u8 byte;
			u32 nShift = 0;
			size_t nBytesRead = 0;

			value = 0;

			//-- Always read at least one byte the normal way.
			//-- (to handle the fourCC)
			nBytesRead += SerializeBytes(&byte, 1, nFourCC);
			if (nBytesRead > 0)
			{
				value |= TType(byte & 0x7f) << nShift;
				nShift += 7;
	
				while (byte >= 0x80)
				{
					if (IS_PTR(m_Buffer.StripHead(&byte, 1)))
					{
						value |= TType(byte & 0x7f) << nShift;
						nShift += 7;
						++nBytesRead;
					}
					else
					{
						//-- Error in read. Manually set m_eError.
						m_eError = ConvertError(m_Buffer.GetError());
						byte = 0;
						nBytesRead = 0;
					}


					if (nBytesRead > (sizeof(u32) + sizeof(u64)))
					{
						//-- Safety test - we can never read more than 12 bytes. (fourcc + u64)
						//-- With LEB128 it should be less, but if we ever read more than 12,
						//-- we know something is wrong and we can stop.
						m_eError = Error::SizeMismatch;
						byte = 0;
						nBytesRead = 0;
					}
				} 
			}
			else
			{
				//-- Error in read. m_eError already set by SerializeBytes().
			}

			return nBytesRead;
		}

		Error::Enum				ConvertError(ISimpleBuffer::Error::Enum e);

		ISimpleBuffer&			m_Buffer;
		Error::Enum				m_eError;
		bool					m_bDecompress;
		bool					m_bExpectFourCCs;
};


//----------------------------------------------------------//
// EXTERNALS
//----------------------------------------------------------//

//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//

#endif //_SIMPLE_BUFFER_DESERIALIZER_H_