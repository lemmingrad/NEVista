//----------------------------------------------------------//
// SIMPLEBUFFERDESERIALIZER.CPP
//----------------------------------------------------------//
//-- Description
// Serializing class for deserializing from a CSimpleBuffer.
//----------------------------------------------------------//


#include "SimpleBufferDeserializer.h"
#include "Types.h"
#include <vector>
#include "IFixedString.h"


//----------------------------------------------------------//
// DEFINES
//----------------------------------------------------------//

//----------------------------------------------------------//
// GLOBALS
//----------------------------------------------------------//

//----------------------------------------------------------//
// CSimpleBufferDeserializer::CSimpleBufferDeserializer
//----------------------------------------------------------//
CSimpleBufferDeserializer::CSimpleBufferDeserializer(ISimpleBuffer& Buffer, bool bDecompress, bool bExpectFourCCs)
: CSerializer(CSerializer::Mode::Deserializing)
, m_Buffer(Buffer)
, m_eError(Error::Ok)
, m_bDecompress(bDecompress)
, m_bExpectFourCCs(bExpectFourCCs)
{
}


//----------------------------------------------------------//
// CSimpleBufferDeserializer::~CSimpleBufferDeserializer
//----------------------------------------------------------//
CSimpleBufferDeserializer::~CSimpleBufferDeserializer()
{
}


//----------------------------------------------------------//
// CSimpleBufferDeserializer::GetError
//----------------------------------------------------------//
CSimpleBufferDeserializer::Error::Enum CSimpleBufferDeserializer::GetError() const
{
	return m_eError;
}


//----------------------------------------------------------//
// CSimpleBufferDeserializer::SerializeBytes
//----------------------------------------------------------//
// Serialize a number of bytes.
// Also does debug FourCC if required.
//----------------------------------------------------------//
size_t CSimpleBufferDeserializer::SerializeBytes(u8* pData, size_t nDataSize, u32 nFourCC)
{
	m_eError = Error::Ok;

	if (IS_PTR(pData)
		&& (nDataSize > 0))
	{
		size_t nActualSize = 0;
		size_t nExpectedSize = nDataSize;
		if (IS_TRUE(m_bExpectFourCCs))
		{
			nExpectedSize += sizeof(UBAFourCC);
		}

		if (m_Buffer.UsedSize() >= nExpectedSize)
		{
			if (IS_TRUE(m_bExpectFourCCs))
			{
				UBAFourCC FCC;
	
				if (IS_PTR(m_Buffer.StripHead(FCC.m_Bytes, sizeof(FCC.m_Bytes))))
				{
					//-- Success.
					nActualSize += sizeof(FCC.m_Bytes);

					//-- If we are using FourCCs, make sure they appear in memory in a readable format.
					//-- That means reversing the byte order on little-endian systems.
					if (IS_FALSE(SysMemory::IsSystemBigEndian()))
					{
						//-- little-endian system. Convert data to big-endian
						SysMemory::EndianSwap(FCC.m_Bytes, sizeof(FCC.m_Bytes));
					}

					assert(nFourCC == FCC.m_nValue);
					if (nFourCC != FCC.m_nValue)
					{
						//-- FourCC mismatch!
						m_eError = Error::FourCCMismatch;
						return 0;
					}
				}
				else
				{
					//-- Failed.
					m_eError = ConvertError(m_Buffer.GetError());
				}
			}

			if (Error::Ok == m_eError)
			{
				if (IS_PTR(m_Buffer.StripHead(pData, nDataSize)))
				{
					//-- Success.
					nActualSize += nDataSize;

					if (nActualSize == nExpectedSize)
					{
						return nActualSize;
					}
					else
					{
						//-- Error occurred.
						m_eError = Error::SizeMismatch;
					}
				}
				else
				{
					//-- Failed.
					m_eError = ConvertError(m_Buffer.GetError());
				}
			}
		}
		else
		{
			m_eError = Error::WouldUnderflow;
		}
	}
	else
	{
		m_eError = Error::BadParameter;
	}

	return 0;
}

	
//----------------------------------------------------------//
// CSimpleBufferDeserializer::SerializeF32
//----------------------------------------------------------//
size_t CSimpleBufferDeserializer::SerializeF32(f32& fValue, u32 nFourCC)
{
	UBAf32 convertor;

	size_t nActualSize = SerializeBytes(convertor.m_Bytes, sizeof(convertor.m_Bytes), nFourCC);
	if (IS_NOT_ZERO(nActualSize))
	{
		fValue = convertor.m_fValue;
	}

	return nActualSize;
}


//----------------------------------------------------------//
// CSimpleBufferDeserializer::SerializeF64
//----------------------------------------------------------//
size_t CSimpleBufferDeserializer::SerializeF64(f64& fValue, u32 nFourCC)
{
	UBAf64 convertor;

	size_t nActualSize = SerializeBytes(convertor.m_Bytes, sizeof(convertor.m_Bytes), nFourCC);
	if (IS_NOT_ZERO(nActualSize))
	{
		fValue = convertor.m_fValue;
	}

	return nActualSize;
}


//----------------------------------------------------------//
// CSimpleBufferDeserializer::SerializeS32
//----------------------------------------------------------//
size_t CSimpleBufferDeserializer::SerializeS32(s32& nValue, u32 nFourCC)
{
	UBAs32 convertor;

	size_t nActualSize = 0;
	if (IS_TRUE(m_bDecompress))
	{
		nActualSize = SerializeSignedDecompressed<s32>(nValue, nFourCC);
	}
	else
	{
		nActualSize = SerializeBytes(convertor.m_Bytes, sizeof(convertor.m_Bytes), nFourCC);
		if (IS_NOT_ZERO(nActualSize))
		{
			nValue = convertor.m_nValue;
		}
	}

	return nActualSize;
}


//----------------------------------------------------------//
// CSimpleBufferDeserializer::SerializeU32
//----------------------------------------------------------//
size_t CSimpleBufferDeserializer::SerializeU32(u32& nValue, u32 nFourCC)
{
	UBAu32 convertor;

	size_t nActualSize = 0;
	if (IS_TRUE(m_bDecompress))
	{
		nActualSize = SerializeSignedDecompressed<u32>(nValue, nFourCC);
	}
	else
	{
		nActualSize = SerializeBytes(convertor.m_Bytes, sizeof(convertor.m_Bytes), nFourCC);
		if (IS_NOT_ZERO(nActualSize))
		{
			nValue = convertor.m_nValue;
		}
	}

	return nActualSize;
}


//----------------------------------------------------------//
// CSimpleBufferDeserializer::SerializeS8
//----------------------------------------------------------//
size_t CSimpleBufferDeserializer::SerializeS8(s8& nValue, u32 nFourCC)
{
	u8* pByte = (u8*)&nValue;

	return SerializeBytes(pByte, sizeof(pByte), nFourCC);
}


//----------------------------------------------------------//
// CSimpleBufferDeserializer::SerializeU8
//----------------------------------------------------------//
size_t CSimpleBufferDeserializer::SerializeU8(u8& nValue, u32 nFourCC)
{
	u8* pByte = (u8*)&nValue;

	return SerializeBytes(pByte, sizeof(pByte), nFourCC);
}


//----------------------------------------------------------//
// CSimpleBufferDeserializer::SerializeS16
//----------------------------------------------------------//
size_t CSimpleBufferDeserializer::SerializeS16(s16& nValue, u32 nFourCC)
{
	UBAs16 convertor;

	size_t nActualSize = 0;
	if (IS_TRUE(m_bDecompress))
	{
		nActualSize = SerializeSignedDecompressed<s16>(nValue, nFourCC);
	}
	else
	{
		nActualSize = SerializeBytes(convertor.m_Bytes, sizeof(convertor.m_Bytes), nFourCC);
		if (IS_NOT_ZERO(nActualSize))
		{
			nValue = convertor.m_nValue;
		}
	}

	return nActualSize;
}


//----------------------------------------------------------//
// CSimpleBufferDeserializer::SerializeU16
//----------------------------------------------------------//
size_t CSimpleBufferDeserializer::SerializeU16(u16& nValue, u32 nFourCC)
{
	UBAu16 convertor;

	size_t nActualSize = 0;
	if (IS_TRUE(m_bDecompress))
	{
		nActualSize = SerializeSignedDecompressed<u16>(nValue, nFourCC);
	}
	else
	{
		nActualSize = SerializeBytes(convertor.m_Bytes, sizeof(convertor.m_Bytes), nFourCC);
		if (IS_NOT_ZERO(nActualSize))
		{
			nValue = convertor.m_nValue;
		}
	}

	return nActualSize;
}


//----------------------------------------------------------//
// CSimpleBufferDeserializer::SerializeS64
//----------------------------------------------------------//
size_t CSimpleBufferDeserializer::SerializeS64(s64& nValue, u32 nFourCC)
{
	UBAs64 convertor;

	size_t nActualSize = 0;
	if (IS_TRUE(m_bDecompress))
	{
		nActualSize = SerializeSignedDecompressed<s64>(nValue, nFourCC);
	}
	else
	{
		nActualSize = SerializeBytes(convertor.m_Bytes, sizeof(convertor.m_Bytes), nFourCC);
		if (IS_NOT_ZERO(nActualSize))
		{
			nValue = convertor.m_nValue;
		}
	}

	return nActualSize;
}


//----------------------------------------------------------//
// CSimpleBufferDeserializer::SerializeU64
//----------------------------------------------------------//
size_t CSimpleBufferDeserializer::SerializeU64(u64& nValue, u32 nFourCC)
{
	UBAu64 convertor;

	size_t nActualSize = 0;
	if (IS_TRUE(m_bDecompress))
	{
		nActualSize = SerializeSignedDecompressed<u64>(nValue, nFourCC);
	}
	else
	{
		nActualSize = SerializeBytes(convertor.m_Bytes, sizeof(convertor.m_Bytes), nFourCC);
		if (IS_NOT_ZERO(nActualSize))
		{
			nValue = convertor.m_nValue;
		}
	}

	return nActualSize;
}


//----------------------------------------------------------//
// CSimpleBufferDeserializer::SerializeBitfield
//----------------------------------------------------------//
size_t CSimpleBufferDeserializer::SerializeBitfield(bitfield& nFlags, u32 nFourCC)
{
	UBAbitfield convertor;

	size_t nActualSize = 0;
	if (IS_TRUE(m_bDecompress))
	{
		nActualSize = SerializeSignedDecompressed<bitfield>(nFlags, nFourCC);
	}
	else
	{
		nActualSize = SerializeBytes(convertor.m_Bytes, sizeof(convertor.m_Bytes), nFourCC);
		if (IS_NOT_ZERO(nActualSize))
		{
			nFlags = convertor.m_nValue;
		}
	}

	return nActualSize;
}


//----------------------------------------------------------//
// CSimpleBufferDeserializer::SerializeBool
//----------------------------------------------------------//
size_t CSimpleBufferDeserializer::SerializeBool(bool& bValue, u32 nFourCC)
{
	u8 nByte = 0;

	size_t nActualBytes = SerializeU8(nByte, nFourCC);
	if (IS_NOT_ZERO(nActualBytes))
	{
		bValue = IS_ZERO(nByte) ? false : true;
	}

	return nActualBytes;
}


//----------------------------------------------------------//
// CSimpleBufferDeserializer::SerializeString
//----------------------------------------------------------//
size_t CSimpleBufferDeserializer::SerializeString(std::string& strng, u32 nFourCC)
{
	u32 nSize = 0;

	size_t nActualSize = SerializeU32(nSize, 'slen');
	if (IS_NOT_ZERO(nActualSize))
	{
		std::vector<s8> tempStore(nSize);

		size_t nRead = SerializeBytes((u8*)&tempStore[0], nSize, nFourCC);
		if (IS_NOT_ZERO(nRead))
		{
			strng.assign(tempStore.begin(), tempStore.end());
			nActualSize += nRead;
		}
	}

	return nActualSize;
}


//----------------------------------------------------------//
// CSimpleBufferDeserializer::SerializeFixedString
//----------------------------------------------------------//
size_t CSimpleBufferDeserializer::SerializeFixedString(IFixedString& fixedString, u32 nFourCC)
{
	u32 nSize = 0;

	size_t nActualSize = SerializeU32(nSize, 'slen');
	if (IS_NOT_ZERO(nActualSize))
	{
		std::vector<s8> tempStore(nSize);

		size_t nRead = SerializeBytes((u8*)&tempStore[0], nSize, nFourCC);
		if (IS_NOT_ZERO(nRead))
		{
			std::string tempString(tempStore.begin(), tempStore.end());
			fixedString.Set(tempString.c_str());

			nActualSize += nRead;
		}
	}

	return nActualSize;
}


//----------------------------------------------------------//
// CSimpleBufferDeserializer::ConvertError
//----------------------------------------------------------//
CSimpleBufferDeserializer::Error::Enum CSimpleBufferDeserializer::ConvertError(ISimpleBuffer::Error::Enum e)
{
	switch (e)
	{
		case ISimpleBuffer::Error::MoveFailed:
		{
			return Error::MoveFailed;
		}
		break;
		case ISimpleBuffer::Error::CopyFailed:
		{
			return Error::CopyFailed;
		}
		break;
		case ISimpleBuffer::Error::BadParameter:
		{
			return Error::BadParameter;
		}
		break;
		case ISimpleBuffer::Error::WouldUnderflow:
		{
			return Error::WouldUnderflow;
		}
		break;
		case ISimpleBuffer::Error::WouldOverflow:
		{
			return Error::WouldOverflow;
		}
		break;
		case ISimpleBuffer::Error::Fail:
		default:
		{
			return Error::Fail;
		}
		break;
		case ISimpleBuffer::Error::Ok:
		{
			return Error::Ok;
		}
		break;
	}
}


//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//
