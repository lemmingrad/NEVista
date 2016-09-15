//----------------------------------------------------------//
// SIMPLEBUFFERSERIALIZER.CPP
//----------------------------------------------------------//
//-- Description
// Serializing class for serializing into a CSimpleBuffer.
//----------------------------------------------------------//


#include "SimpleBufferSerializer.h"
#include "Types.h"


//----------------------------------------------------------//
// DEFINES
//----------------------------------------------------------//

//----------------------------------------------------------//
// GLOBALS
//----------------------------------------------------------//

//----------------------------------------------------------//
// CSimpleBufferSerializer::CSimpleBufferSerializer
//----------------------------------------------------------//
CSimpleBufferSerializer::CSimpleBufferSerializer(CSimpleBuffer& Buffer, bool bCompress, bool bIncludeFourCCs)
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
// CSimpleBufferSerializer::SerializeSignedCompressed
//----------------------------------------------------------//
// Compress and serialize an integer of bytes using LEB128.
// Input value should be 16-bit or more.
//----------------------------------------------------------//
template<class TType>
size_t CSimpleBufferSerializer::SerializeSignedCompressed<TType>(TType& value, u32 nFourCC)
{
	m_eError = Error::Ok;

	if (IS_PTR(pData)
		&& (nDataSize > 0))
	{
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
	else
	{
		m_eError = Error::BadParameter;
	}

	return 0;
}


//----------------------------------------------------------//
// CSimpleBufferSerializer::SerializeUnsignedCompressed
//----------------------------------------------------------//
// Compress and serialize an integer using ULEB128.
// Input value should be 16-bit or more.
//----------------------------------------------------------//
template<class TType>
size_t CSimpleBufferSerializer::SerializeUnsignedCompressed<TType>(TType& value, u32 nFourCC)
{
	m_eError = Error::Ok;

	if (IS_PTR(pData)
		&& (nDataSize > 0))
	{
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
	else
	{
		m_eError = Error::BadParameter;
	}

	return 0;
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
		size_t nRequired = nDataSize;
		if (IS_TRUE(m_bIncludeFourCCs))
		{
			nRequired += sizeof(UBAFourCC);
		}

		if (m_Buffer.UnusedSize() >= nRequired)
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
					SysMemory::EndianSwap(FCC.m_Bytes, sizeof(FCC.m_nBytes));
				}

				if (IS_PTR(m_Buffer.InsTail(&FCC.m_Bytes, sizeof(FCC.m_nBytes))))
				{
					//-- Success.
					m_eError = Error::Ok;
				}
				else
				{
					//-- Failed.
					m_eError = ConvertError(m_buffer.GetError());
				}
			}

			if (Error::Ok == m_eError)
			{
				if (IS_PTR(m_Buffer.InsTail(pData, nDataSize)))
				{
					//-- Success.
					m_eError = Error::Ok;
				}
				else
				{
					//-- Failed.
					m_eError = ConvertError(m_buffer.GetError());
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
size_t CPacketSerializer::SerializeString(std::string& strng, u32 nFourCC)
{
	m_eError = Error::Ok;

	u32 nSize = 0;
	size_t nReturn = 0;

	nSize = (u32)strng.length();
	
	nReturn += SerializeU32(nSize, "slen");
	
	if (Error::Ok == m_eError)
	{
		nReturn += SerializeBytes(strng.c_str(), nSize, nFourCC);
	}

	return nReturn;
}


//----------------------------------------------------------//
// CSimpleBufferSerializer::SerializeFixedString
//----------------------------------------------------------//
/*
size_t CSimpleBufferSerializer::SerializeFixedString(IFixedString& fixedString, u32 nFourCC)
{
	u64 nSize = 0;
	size_t nReturn = 0;

	if (Mode::Serializing == m_eMode)
	{
		nSize = (u64)fixedString.Size();
		nReturn += SerializeU64(nSize, "slen");
		if (Error::Ok == m_eError)
		{
			nReturn += SerializeBytes(fixedString.Buffer(), fixedString.Size(), nFourCC);
		}
	}
	else
	{
		nReturn += SerializeU64(nSize, "slen");
		if (Error::Ok == m_Error)
		{
			if (fixedString.Size() >= nSize)
			{
				nReturn += SerializeBytes(fixedString.Buffer(), fixedString.Size(), nFourCC);
			}
			else
			{
				if (IS_PTR(SerializeReserve(nSize)))
				{
					nReturn += nSize;
				}
			}
		}
	}
}
*/


//----------------------------------------------------------//
// CSimpleBufferSerializer::ConvertError
//----------------------------------------------------------//
CSimpleBufferSerializer::Error::Enum CSimpleBufferSerializer::ConvertError(CSimpleBuffer::Error::Enum e)
{
	switch (e)
	{
		case CSimpleBuffer::Error::MoveFailed:
		{
			return Error::MoveFailed;
		}
		break;
		case CSimpleBuffer::Error::CopyFailed
		{
			return Error::CopyFailed;
		}
		break;
		case CSimpleBuffer::Error::BadParameter:
		{
			return Error::BadParameter;
		}
		break;
		case CSimpleBuffer::Error::WouldUnderflow:
		{
			return Error::WouldUnderflow;
		}
		break;
		case CSimpleBuffer::Error::WouldOverflow:
		{
			return Error::WouldOverflow;
		}
		break;
		case CSimpleBuffer::Error::Fail:
		{
			return Error::Fail;
		}
		break;
		case CSimpleBuffer::Error::Ok:
		default:
		{
			return Error::Ok;
		}
		break;
	}
}


//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//
