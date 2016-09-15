//----------------------------------------------------------//
// SIMPLEBUFFERDESERIALIZER.CPP
//----------------------------------------------------------//
//-- Description
// Serializing class for deserializing from a CSimpleBuffer.
//----------------------------------------------------------//


#include "SimpleBufferDeserializer.h"
#include "Types.h"


//----------------------------------------------------------//
// DEFINES
//----------------------------------------------------------//

//----------------------------------------------------------//
// GLOBALS
//----------------------------------------------------------//

//----------------------------------------------------------//
// CSimpleBufferDeserializer::CSimpleBufferDeserializer
//----------------------------------------------------------//
CSimpleBufferDeserializer::CSimpleBufferDeserializer(CSimpleBuffer& Buffer, bool bDecompress, bool bExpectFourCCs)
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
// CSimpleBufferDeserializer::SerializeSignedDecompressed
//----------------------------------------------------------//
// Decompress and serialize an integer of bytes using LEB128.
// Input value should be 16-bit or more.
//----------------------------------------------------------//
template<class TType>
size_t CSimpleBufferDeserializer::SerializeSignedDecompressed<TType>(TType& value, u32 nFourCC)
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
// CSimpleBufferDeserializer::SerializeUnsignedDecompressed
//----------------------------------------------------------//
// Decompress and serialize an integer using ULEB128.
// Input value should be 16-bit or more.
//----------------------------------------------------------//
template<class TType>
size_t CSimpleBufferDeserializer::SerializeUnsignedDecompressed<TType>(TType& value, u32 nFourCC)
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

/*
		template<typename T_Visitor, typename T_Type>
		static void decodeULEB128(T_Visitor& visitor, const char* name, T_Type& value)
		{
			uint32 shift = 0;
			uint8 encodedByte;

			value = 0;
			do
			{
				visitor.Serialize(name, encodedByte);
				value |= T_Type(encodedByte & 0x7f) << shift;
				shift += 7;
			} while (encodedByte >= 0x80);
		}

		template<typename T_Visitor, typename T_Type>
		static void decodeSLEB128(T_Visitor& visitor, const char* name, T_Type& value)
		{
			uint32 shift = 0;
			uint8 encodedByte;

			value = 0;
			do
			{
				visitor.Serialize(name, encodedByte);
				value |= T_Type(encodedByte & 0x7f) << shift;
				shift += 7;
			} while (encodedByte >= 0x80);

			// Sign extend negative numbers
			if ((shift < 8 * sizeof(T_Type)) && (encodedByte & 0x40))
				value |= T_Type(-1) << shift;
		}
*/


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
		size_t nRequired = nDataSize;
		if (IS_TRUE(m_bExpectFourCCs))
		{
			nRequired += sizeof(UBAFourCC);
		}

		if (m_Buffer.UnusedSize() >= nRequired)
		{
			if (IS_TRUE(m_bExpectFourCCs))
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

	/*
#if defined(PACKET_SERIALIZER_USES_DEBUG_FOURCC)
				//-- FourCC is compared to input if present.
				if (IS_PTR(SysMemory::Memcpy(FCC.m_Bytes, nFourCCSize, pBuffer, nFourCCSize)))
				{
					m_nOffset += nFourCCSize;
					nRemaining -= nFourCCSize;
					pBuffer += nFourCCSize;

					if (IS_FALSE(SysMemory::IsSystemBigEndian()))
					{
						//-- little-endian system. Convert data from big-endian
						SysMemory::EndianSwap(FCC.m_Bytes, nFourCCSize);
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
					m_eError = Error::CopyFailed;
					return 0;
				}
#endif //PACKET_SERIALIZER_USES_DEBUG_FOURCC

				//-- Mode::Deserializing, buffer to output variables.
				if (IS_PTR(SysMemory::Memcpy(pData, nDataSize, pBuffer, nDataSize)))
				{
					m_nOffset += nDataSize;
					return nRequired;
				}
				else
				{
					m_eError = Error::CopyFailed;
					return 0;
				}
			}
		}
		else
		{
			m_eError = Error::EndReached;
		}
	}
	else
	{
		m_eError = Error::BadParameter;
	}

	//-- Failed
	return 0;
}
	*/	
	
//----------------------------------------------------------//
// CSimpleBufferDeserializer::SerializeF32
//----------------------------------------------------------//
size_t CSimpleBufferDeserializer::SerializeF32(f32& fValue, u32 nFourCC)
{
	UBAf32 convertor;

	size_t nExpectedSize = sizeof(convertor.m_Bytes);
	if (IS_TRUE(m_bExpectFourCCs))
	{
		nExpectedSize += sizeof(UBAFourCC);
	}

	size_t nActualSize = SerializeBytes(convertor.m_Bytes, sizeof(convertor.m_Bytes), nFourCC);
	if (nActualSize == nExpectedSize)
	{
		m_eError = Error::Ok;

		fValue = convertor.m_fValue;
		return nActualSize;
	}
	else
	{
		//-- Error occurred.
		m_eError = ConvertError(m_Buffer.GetError());
	}

	return 0;
}


//----------------------------------------------------------//
// CSimpleBufferDeserializer::SerializeF64
//----------------------------------------------------------//
size_t CSimpleBufferDeserializer::SerializeF64(f64& fValue, u32 nFourCC)
{
	UBAf64 convertor;

	size_t nExpectedSize = sizeof(convertor.m_Bytes);
	if (IS_TRUE(m_bExpectFourCCs))
	{
		nExpectedSize += sizeof(UBAFourCC);
	}

	size_t nActualSize = SerializeBytes(convertor.m_Bytes, sizeof(convertor.m_Bytes), nFourCC);
	if (nActualSize == nExpectedSize)
	{
		m_eError = Error::Ok;

		fValue = convertor.m_fValue;
		return nActualSize;
	}
	else
	{
		//-- Error occurred.
		m_eError = ConvertError(m_Buffer.GetError());
	}

	return 0;
}


//----------------------------------------------------------//
// CSimpleBufferDeserializer::SerializeS32
//----------------------------------------------------------//
size_t CSimpleBufferDeserializer::SerializeS32(s32& nValue, u32 nFourCC)
{
	if (IS_TRUE(m_bDecompress))
	{
		return SerializeSignedDecompressed<s32>(nValue, nFourCC);
	}
	else
	{
		UBAs32 convertor;
		convertor.m_nValue = nValue;

		return SerializeBytes(convertor.m_Bytes, sizeof(convertor.m_Bytes), nFourCC);
	}
}


//----------------------------------------------------------//
// CSimpleBufferDeserializer::SerializeU32
//----------------------------------------------------------//
size_t CSimpleBufferDeserializer::SerializeU32(u32& nValue, u32 nFourCC)
{
	if (IS_TRUE(m_bDecompress))
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
// CSimpleBufferDeserializer::SerializeS8
//----------------------------------------------------------//
size_t CSimpleBufferDeserializer::SerializeS8(s8& nValue, u32 nFourCC)
{
	u8* pByte = (u8*)&nValue;

	size_t nExpectedSize = sizeof(pByte);
	if (IS_TRUE(m_bExpectFourCCs))
	{
		nExpectedSize += sizeof(UBAFourCC);
	}

	size_t nActualSize = SerializeBytes(pByte, sizeof(pByte), nFourCC);
	if (nActualSize == nExpectedSize)
	{
		m_eError = Error::Ok;

		fValue = convertor.m_fValue;
		return nActualSize;
	}
	else
	{
		//-- Error occurred.
		m_eError = ConvertError(m_Buffer.GetError());
	}

	return 0;
}

/*
	{
		size_t nExpectedSize = nConvertorSize;
#if defined(PACKET_SERIALIZER_USES_DEBUG_FOURCC)
		nExpectedSize += sizeof(UBAFourCC);
#endif //PACKET_SERIALIZER_USES_DEBUG_FOURCC

		if (nExpectedSize == SerializeBytes(pByte, nConvertorSize, nFourCC))
		{
			return nExpectedSize;
		}
	}
*/


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
	if (IS_TRUE(m_bDecompress))
	{
		return SerializeSignedDecompressed<s16>(nValue, nFourCC);
	}
	else
	{
		UBAs16 convertor;
		convertor.m_nValue = nValue;

		return SerializeBytes(convertor.m_Bytes, sizeof(convertor.m_Bytes), nFourCC);
	}
}


//----------------------------------------------------------//
// CSimpleBufferDeserializer::SerializeU16
//----------------------------------------------------------//
size_t CSimpleBufferDeserializer::SerializeU16(u16& nValue, u32 nFourCC)
{
	if (IS_TRUE(m_bDecompress))
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
// CSimpleBufferDeserializer::SerializeS64
//----------------------------------------------------------//
size_t CSimpleBufferDeserializer::SerializeS64(s64& nValue, u32 nFourCC)
{
	if (IS_TRUE(m_bDecompress))
	{
		return SerializeSignedDecompressed<s64>(nValue, nFourCC);
	}
	else
	{
		UBAs64 convertor;
		convertor.m_nValue = nValue;

		return SerializeBytes(convertor.m_Bytes, sizeof(convertor.m_Bytes), nFourCC);
	}
}


//----------------------------------------------------------//
// CSimpleBufferDeserializer::SerializeU64
//----------------------------------------------------------//
size_t CSimpleBufferDeserializer::SerializeU64(u64& nValue, u32 nFourCC)
{
	if (IS_TRUE(m_bDecompress))
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
// CSimpleBufferDeserializer::SerializeBitfield
//----------------------------------------------------------//
size_t CSimpleBufferDeserializer::SerializeBitfield(bitfield& nFlags, u32 nFourCC)
{
	UBAbitfield convertor;
	convertor.m_nValue = nFlags;

	if (IS_TRUE(m_bDecompress))
	{
		return SerializeUnsignedCompressed<bitfield>(convertor.m_nValue, nFourCC);
	}
	else
	{
		return SerializeBytes(convertor.m_Bytes, sizeof(convertor.m_Bytes), nFourCC);
	}
}


//----------------------------------------------------------//
// CSimpleBufferDeserializer::SerializeBool
//----------------------------------------------------------//
size_t CSimpleBufferDeserializer::SerializeBool(bool& bValue, u32 nFourCC)
{
	u8 nByte;
	nByte = IS_TRUE(bValue) ? 1 : 0;

	return SerializeU8(nByte, nFourCC);
}


//----------------------------------------------------------//
// CSimpleBufferDeserializer::SerializeString
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
/*
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
*/

	return nReturn;
}


//----------------------------------------------------------//
// CSimpleBufferDeserializer::SerializeFixedString
//----------------------------------------------------------//
/*
size_t CSimpleBufferDeserializer::SerializeFixedString(IFixedString& fixedString, u32 nFourCC)
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
// CSimpleBufferDeserializer::ConvertError
//----------------------------------------------------------//
CSimpleBufferDeserializer::Error::Enum CSimpleBufferDeserializer::ConvertError(CSimpleBuffer::Error::Enum e)
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
