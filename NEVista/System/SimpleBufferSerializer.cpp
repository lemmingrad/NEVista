//----------------------------------------------------------//
// SIMPLEBUFFERSERIALIZER.CPP
//----------------------------------------------------------//
//-- Description
// Serializing class for serializing into a CSimpleBuffer.
//----------------------------------------------------------//


#include "SimpleBufferSerializer.h"
#include "Types.h"
#include "IFixedString.h"


//----------------------------------------------------------//
// DEFINES
//----------------------------------------------------------//

//----------------------------------------------------------//
// GLOBALS
//----------------------------------------------------------//

//----------------------------------------------------------//
// CSimpleBufferSerializer::CSimpleBufferSerializer
//----------------------------------------------------------//
CSimpleBufferSerializer::CSimpleBufferSerializer(ISimpleBuffer& Buffer, bool bCompress, bool bIncludeFourCCs)
: CSerializer(CSerializer::Mode::Serializing)
, m_Buffer(Buffer)
, m_eError(Error::Ok)
, m_bCompress(bCompress)
, m_bIncludeFourCCs(bIncludeFourCCs)
{
}


//----------------------------------------------------------//
// CSimpleBufferSerializer::~CSimpleBufferSerializer
//----------------------------------------------------------//
CSimpleBufferSerializer::~CSimpleBufferSerializer()
{
}


//----------------------------------------------------------//
// CSimpleBufferSerializer::GetError
//----------------------------------------------------------//
CSimpleBufferSerializer::Error::Enum CSimpleBufferSerializer::GetError() const
{
	return m_eError;
}


//----------------------------------------------------------//
// CSimpleBufferSerializer::SerializeBytes
//----------------------------------------------------------//
// Serialize a number of bytes.
// Also does debug FourCC if required.
//----------------------------------------------------------//
size_t CSimpleBufferSerializer::SerializeBytes(u8* pData, size_t nDataSize, u32 nFourCC)
{
	m_eError = Error::Ok;

	if (IS_PTR(pData)
		&& (nDataSize > 0))
	{
		size_t nActualSize = 0;
		size_t nRequiredSize = nDataSize;
		if (IS_TRUE(m_bIncludeFourCCs))
		{
			nRequiredSize += sizeof(UBAFourCC);
		}

		if (m_Buffer.UnusedSize() >= nRequiredSize)
		{
			if (IS_TRUE(m_bIncludeFourCCs))
			{
				UBAFourCC FCC;
				FCC.m_nValue = nFourCC;
				//-- If we are using FourCCs, make sure they appear in memory in a readable format.
				//-- That means reversing the byte order on little-endian systems.
				if (IS_FALSE(SysMemory::IsSystemBigEndian()))
				{
					//-- little-endian system. Convert data to big-endian
					SysMemory::EndianSwap(FCC.m_Bytes, sizeof(FCC.m_Bytes));
				}

				if (IS_PTR(m_Buffer.InsTail(FCC.m_Bytes, sizeof(FCC.m_Bytes))))
				{
					//-- Success.
					nActualSize += sizeof(FCC.m_Bytes);
				}
				else
				{
					//-- Failed.
					m_eError = ConvertError(m_Buffer.GetError());
				}
			}

			if (Error::Ok == m_eError)
			{
				if (IS_PTR(m_Buffer.InsTail(pData, nDataSize)))
				{
					//-- Success.
					nActualSize += nDataSize;

					if (nActualSize == nRequiredSize)
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
			m_eError = Error::WouldOverflow;
		}
	}
	else
	{
		m_eError = Error::BadParameter;
	}

	return 0;
}

	
//----------------------------------------------------------//
// CSimpleBufferSerializer::SerializeF32
//----------------------------------------------------------//
size_t CSimpleBufferSerializer::SerializeF32(f32& fValue, u32 nFourCC)
{
	UBAf32 convertor;
	convertor.m_fValue = fValue;

	return SerializeBytes(convertor.m_Bytes, sizeof(convertor.m_Bytes), nFourCC);
}


//----------------------------------------------------------//
// CSimpleBufferSerializer::SerializeF64
//----------------------------------------------------------//
size_t CSimpleBufferSerializer::SerializeF64(f64& fValue, u32 nFourCC)
{
	UBAf64 convertor;
	convertor.m_fValue = fValue;

	return SerializeBytes(convertor.m_Bytes, sizeof(convertor.m_Bytes), nFourCC);
}


//----------------------------------------------------------//
// CSimpleBufferSerializer::SerializeS32
//----------------------------------------------------------//
size_t CSimpleBufferSerializer::SerializeS32(s32& nValue, u32 nFourCC)
{
	if (IS_TRUE(m_bCompress))
	{
		return SerializeSignedCompressed<s32>(nValue, nFourCC);
	}
	else
	{
		UBAs32 convertor;
		convertor.m_nValue = nValue;

		return SerializeBytes(convertor.m_Bytes, sizeof(convertor.m_Bytes), nFourCC);
	}
}


//----------------------------------------------------------//
// CSimpleBufferSerializer::SerializeU32
//----------------------------------------------------------//
size_t CSimpleBufferSerializer::SerializeU32(u32& nValue, u32 nFourCC)
{
	if (IS_TRUE(m_bCompress))
	{
		return SerializeUnsignedCompressed<u32>(nValue, nFourCC);
	}
	else
	{
		UBAu32 convertor;
		convertor.m_nValue = nValue;

		return SerializeBytes(convertor.m_Bytes, sizeof(convertor.m_Bytes), nFourCC);
	}
}


//----------------------------------------------------------//
// CSimpleBufferSerializer::SerializeS8
//----------------------------------------------------------//
size_t CSimpleBufferSerializer::SerializeS8(s8& nValue, u32 nFourCC)
{
	u8* pByte = (u8*)&nValue;

	return SerializeBytes(pByte, sizeof(pByte), nFourCC);
}


//----------------------------------------------------------//
// CSimpleBufferSerializer::SerializeU8
//----------------------------------------------------------//
size_t CSimpleBufferSerializer::SerializeU8(u8& nValue, u32 nFourCC)
{
	u8* pByte = (u8*)&nValue;

	return SerializeBytes(pByte, sizeof(pByte), nFourCC);
}


//----------------------------------------------------------//
// CSimpleBufferSerializer::SerializeS16
//----------------------------------------------------------//
size_t CSimpleBufferSerializer::SerializeS16(s16& nValue, u32 nFourCC)
{
	if (IS_TRUE(m_bCompress))
	{
		return SerializeSignedCompressed<s16>(nValue, nFourCC);
	}
	else
	{
		UBAs16 convertor;
		convertor.m_nValue = nValue;

		return SerializeBytes(convertor.m_Bytes, sizeof(convertor.m_Bytes), nFourCC);
	}
}


//----------------------------------------------------------//
// CSimpleBufferSerializer::SerializeU16
//----------------------------------------------------------//
size_t CSimpleBufferSerializer::SerializeU16(u16& nValue, u32 nFourCC)
{
	if (IS_TRUE(m_bCompress))
	{
		return SerializeUnsignedCompressed<u16>(nValue, nFourCC);
	}
	else
	{
		UBAu16 convertor;
		convertor.m_nValue = nValue;

		return SerializeBytes(convertor.m_Bytes, sizeof(convertor.m_Bytes), nFourCC);
	}
}


//----------------------------------------------------------//
// CSimpleBufferSerializer::SerializeS64
//----------------------------------------------------------//
size_t CSimpleBufferSerializer::SerializeS64(s64& nValue, u32 nFourCC)
{
	if (IS_TRUE(m_bCompress))
	{
		return SerializeSignedCompressed<s64>(nValue, nFourCC);
	}
	else
	{
		UBAs64 convertor;
		convertor.m_nValue = nValue;

		return SerializeBytes(convertor.m_Bytes, sizeof(convertor.m_Bytes), nFourCC);
	}
}


//----------------------------------------------------------//
// CSimpleBufferSerializer::SerializeU64
//----------------------------------------------------------//
size_t CSimpleBufferSerializer::SerializeU64(u64& nValue, u32 nFourCC)
{
	if (IS_TRUE(m_bCompress))
	{
		return SerializeUnsignedCompressed<u64>(nValue, nFourCC);
	}
	else
	{
		UBAu64 convertor;
		convertor.m_nValue = nValue;

		return SerializeBytes(convertor.m_Bytes, sizeof(convertor.m_Bytes), nFourCC);
	}
}


//----------------------------------------------------------//
// CSimpleBufferSerializer::SerializeBitfield
//----------------------------------------------------------//
size_t CSimpleBufferSerializer::SerializeBitfield(bitfield& nFlags, u32 nFourCC)
{
	UBAbitfield convertor;
	convertor.m_nValue = nFlags;

	if (IS_TRUE(m_bCompress))
	{
		return SerializeUnsignedCompressed<bitfield>(convertor.m_nValue, nFourCC);
	}
	else
	{
		return SerializeBytes(convertor.m_Bytes, sizeof(convertor.m_Bytes), nFourCC);
	}
}


//----------------------------------------------------------//
// CSimpleBufferSerializer::SerializeBool
//----------------------------------------------------------//
size_t CSimpleBufferSerializer::SerializeBool(bool& bValue, u32 nFourCC)
{
	u8 nByte;
	nByte = IS_TRUE(bValue) ? 1 : 0;

	return SerializeU8(nByte, nFourCC);
}


//----------------------------------------------------------//
// CSimpleBufferSerializer::SerializeString
//----------------------------------------------------------//
size_t CSimpleBufferSerializer::SerializeString(std::string& strng, u32 nFourCC)
{
	u32 nSize = (u32)strng.length();
	
	size_t nActualSize = SerializeU32(nSize, 'slen');
	if (IS_NOT_ZERO(nActualSize))
	{
		nActualSize += SerializeBytes((u8*)strng.c_str(), strng.length(), nFourCC);
	}

	return nActualSize;
}


//----------------------------------------------------------//
// CSimpleBufferSerializer::SerializeFixedString
//----------------------------------------------------------//
size_t CSimpleBufferSerializer::SerializeFixedString(IFixedString& fixedString, u32 nFourCC)
{
	u32 nSize = (u32)fixedString.Length();

	size_t nActualSize = SerializeU32(nSize, 'slen');
	if (IS_NOT_ZERO(nActualSize))
	{
		nActualSize += SerializeBytes((u8*)fixedString.ConstBuffer(), fixedString.Length(), nFourCC);
	}

	return nActualSize;
}


//----------------------------------------------------------//
// CSimpleBufferSerializer::ConvertError
//----------------------------------------------------------//
CSimpleBufferSerializer::Error::Enum CSimpleBufferSerializer::ConvertError(ISimpleBuffer::Error::Enum e)
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
