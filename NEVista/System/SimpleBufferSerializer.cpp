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
: ISerializer(ISerializer::Mode::Serializing)
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
// CSimpleBufferSerializer::SerializeReserve
//----------------------------------------------------------//
u8* CSimpleBufferSerializer::SerializeReserve(size_t nReservedSize)
{
	u8* pBuffer = m_Buffer.InsTail(NULL, nReservedSize);
	if (IS_PTR(pBuffer))
	{
		//-- Success
		m_eError = Error::Ok;
	}
	else
	{
		//-- Failed
		m_eError = ConvertError(m_buffer.GetError());
	}

	return pBuffer;
}


//----------------------------------------------------------//
// CSimpleBufferSerializer::SerializeBytes
//----------------------------------------------------------//
// Serialize in/out a number of bytes.
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
// CPacketSerializer::SerializeF32
//----------------------------------------------------------//
size_t CPacketSerializer::SerializeF32(f32& fValue, u32 nFourCC)
{
	UBAf32 convertor;
	size_t nConvertorSize = sizeof(convertor);
	convertor.m_fValue = fValue;

	if (m_bCompress)
	{
		return SerializeCompressed<f32>(fValue, u32 nFourCC);
	}
	else
	{
		return SerializeBytes(convertor.m_Bytes, nConvertorSize, nFourCC);
	}
}

	/*
else
	{
		size_t nExpectedSize = nConvertorSize;
#if defined(PACKET_SERIALIZER_USES_DEBUG_FOURCC)
		nExpectedSize += sizeof(UBAFourCC);
#endif //PACKET_SERIALIZER_USES_DEBUG_FOURCC

		if (nExpectedSize == SerializeBytes(convertor.m_Bytes, nConvertorSize, nFourCC))
		{
			if (IS_FALSE(SysMemory::IsSystemBigEndian()))
			{
				//-- little-endian system. Convert data from big-endian
				SysMemory::EndianSwap(convertor.m_Bytes, nConvertorSize);
			}

			fValue = convertor.m_fValue;
			return nExpectedSize;
		}
	}

	//-- Failed.
	if (Error::Ok == m_eError)
	{
		m_eError = Error::Fail;
	}
	return 0;
}
*/

//----------------------------------------------------------//
// CPacketSerializer::SerializeF64
//----------------------------------------------------------//
size_t CPacketSerializer::SerializeF64(f64& fValue, u32 nFourCC)
{
	UBAf64 convertor;

	size_t nConvertorSize = sizeof(convertor);

	if (Mode::Serializing == m_eMode)
	{	
		convertor.m_fValue = fValue;
		
		//-- To be consistent with big-endian network traffic,
		//-- apply endian swap if system is little-endian.
		if (IS_FALSE(SysMemory::IsSystemBigEndian()))
		{
			//-- little-endian system. Convert data to big-endian
			SysMemory::EndianSwap(convertor.m_Bytes, nConvertorSize);
		}
		
		return SerializeBytes(convertor.m_Bytes, nConvertorSize, nFourCC);
	}
	else
	{
		size_t nExpectedSize = nConvertorSize;
#if defined(PACKET_SERIALIZER_USES_DEBUG_FOURCC)
		nExpectedSize += sizeof(UBAFourCC);
#endif //PACKET_SERIALIZER_USES_DEBUG_FOURCC

		if (nExpectedSize == SerializeBytes(convertor.m_Bytes, nConvertorSize, nFourCC))
		{
			if (IS_FALSE(SysMemory::IsSystemBigEndian()))
			{
				//-- little-endian system. Convert data from big-endian
				SysMemory::EndianSwap(convertor.m_Bytes, nConvertorSize);
			}

			fValue = convertor.m_fValue;
			return nExpectedSize;
		}
	}

	//-- Failed.
	if (Error::Ok == m_eError)
	{
		m_eError = Error::Fail;
	}
	return 0;
}


//----------------------------------------------------------//
// CPacketSerializer::SerializeS32
//----------------------------------------------------------//
size_t CPacketSerializer::SerializeS32(s32& nValue, u32 nFourCC)
{
	UBAs32 convertor;

	size_t nConvertorSize = sizeof(convertor);

	if (Mode::Serializing == m_eMode)
	{	
		convertor.m_nValue = nValue;
		
		//-- To be consistent with big-endian network traffic,
		//-- apply endian swap if system is little-endian.
		if (IS_FALSE(SysMemory::IsSystemBigEndian()))
		{
			//-- little-endian system. Convert data to big-endian
			SysMemory::EndianSwap(convertor.m_Bytes, nConvertorSize);
		}
		
		return SerializeBytes(convertor.m_Bytes, nConvertorSize, nFourCC);
	}
	else
	{
		size_t nExpectedSize = nConvertorSize;
#if defined(PACKET_SERIALIZER_USES_DEBUG_FOURCC)
		nExpectedSize += sizeof(UBAFourCC);
#endif //PACKET_SERIALIZER_USES_DEBUG_FOURCC
	
		if (nExpectedSize == SerializeBytes(convertor.m_Bytes, nConvertorSize, nFourCC))
		{
			if (IS_FALSE(SysMemory::IsSystemBigEndian()))
			{
				//-- little-endian system. Convert data from big-endian
				SysMemory::EndianSwap(convertor.m_Bytes, nConvertorSize);
			}

			nValue = convertor.m_nValue;
			return nExpectedSize;
		}
	}

	//-- Failed.
	if (Error::Ok == m_eError)
	{
		m_eError = Error::Fail;
	}
	return 0;
}


//----------------------------------------------------------//
// CPacketSerializer::SerializeU32
//----------------------------------------------------------//
size_t CPacketSerializer::SerializeU32(u32& nValue, u32 nFourCC)
{
	UBAu32 convertor;

	size_t nConvertorSize = sizeof(convertor);

	if (Mode::Serializing == m_eMode)
	{	
		convertor.m_nValue = nValue;
		
		//-- To be consistent with big-endian network traffic,
		//-- apply endian swap if system is little-endian.
		if (IS_FALSE(SysMemory::IsSystemBigEndian()))
		{
			//-- little-endian system. Convert data to big-endian
			SysMemory::EndianSwap(convertor.m_Bytes, nConvertorSize);
		}
		
		return SerializeBytes(convertor.m_Bytes, nConvertorSize, nFourCC);
	}
	else
	{
		size_t nExpectedSize = nConvertorSize;
#if defined(PACKET_SERIALIZER_USES_DEBUG_FOURCC)
		nExpectedSize += sizeof(UBAFourCC);
#endif //PACKET_SERIALIZER_USES_DEBUG_FOURCC
	
		if (nExpectedSize == SerializeBytes(convertor.m_Bytes, nConvertorSize, nFourCC))
		{
			if (IS_FALSE(SysMemory::IsSystemBigEndian()))
			{
				//-- little-endian system. Convert data from big-endian
				SysMemory::EndianSwap(convertor.m_Bytes, nConvertorSize);
			}

			nValue = convertor.m_nValue;
			return nExpectedSize;
		}
	}

	//-- Failed.
	if (Error::Ok == m_eError)
	{
		m_eError = Error::Fail;
	}
	return 0;
}


//----------------------------------------------------------//
// CPacketSerializer::SerializeS8
//----------------------------------------------------------//
size_t CPacketSerializer::SerializeS8(s8& nValue, u32 nFourCC) 
{
	u8* pByte = (u8*)&nValue;

	size_t nConvertorSize = sizeof(u8);

	if (Mode::Serializing == m_eMode)
	{	
		return SerializeBytes(pByte, nConvertorSize, nFourCC);
	}
	else
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

	//-- Failed.
	if (Error::Ok == m_eError)
	{
		m_eError = Error::Fail;
	}
	return 0;
}


//----------------------------------------------------------//
// CPacketSerializer::SerializeU8
//----------------------------------------------------------//
size_t CPacketSerializer::SerializeU8(u8& nValue, u32 nFourCC)
{
	u8* pByte = (u8*)&nValue;

	size_t nConvertorSize = sizeof(u8);

	if (Mode::Serializing == m_eMode)
	{	
		return SerializeBytes(pByte, nConvertorSize, nFourCC);
	}
	else
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

	//-- Failed.
	if (Error::Ok == m_eError)
	{
		m_eError = Error::Fail;
	}
	return 0;
}


//----------------------------------------------------------//
// CPacketSerializer::SerializeS16
//----------------------------------------------------------//
size_t CPacketSerializer::SerializeS16(s16& nValue, u32 nFourCC) 
{
	UBAs16 convertor;
	
	size_t nConvertorSize = sizeof(convertor);

	if (Mode::Serializing == m_eMode)
	{	
		convertor.m_nValue = nValue;
		
		//-- To be consistent with big-endian network traffic,
		//-- apply endian swap if system is little-endian.
		if (IS_FALSE(SysMemory::IsSystemBigEndian()))
		{
			//-- little-endian system. Convert data to big-endian
			SysMemory::EndianSwap(convertor.m_Bytes, nConvertorSize);
		}
		
		return SerializeBytes(convertor.m_Bytes, nConvertorSize, nFourCC);
	}
	else
	{
		size_t nExpectedSize = nConvertorSize;
#if defined(PACKET_SERIALIZER_USES_DEBUG_FOURCC)
		nExpectedSize += sizeof(UBAFourCC);
#endif //PACKET_SERIALIZER_USES_DEBUG_FOURCC

		if (nExpectedSize == SerializeBytes(convertor.m_Bytes, nConvertorSize, nFourCC))
		{
			if (IS_FALSE(SysMemory::IsSystemBigEndian()))
			{
				//-- little-endian system. Convert data from big-endian
				SysMemory::EndianSwap(convertor.m_Bytes, nConvertorSize);
			}

			nValue = convertor.m_nValue;
			return nExpectedSize;
		}
	}

	//-- Failed.
	if (Error::Ok == m_eError)
	{
		m_eError = Error::Fail;
	}
	return 0;
}


//----------------------------------------------------------//
// CPacketSerializer::SerializeU16
//----------------------------------------------------------//
size_t CPacketSerializer::SerializeU16(u16& nValue, u32 nFourCC) 
{
	UBAu16 convertor;

	size_t nConvertorSize = sizeof(convertor);

	if (Mode::Serializing == m_eMode)
	{	
		convertor.m_nValue = nValue;
		
		//-- To be consistent with big-endian network traffic,
		//-- apply endian swap if system is little-endian.
		if (IS_FALSE(SysMemory::IsSystemBigEndian()))
		{
			//-- little-endian system. Convert data to big-endian
			SysMemory::EndianSwap(convertor.m_Bytes, nConvertorSize);
		}
		
		return SerializeBytes(convertor.m_Bytes, nConvertorSize, nFourCC);
	}
	else
	{
		size_t nExpectedSize = nConvertorSize;
#if defined(PACKET_SERIALIZER_USES_DEBUG_FOURCC)
		nExpectedSize += sizeof(UBAFourCC);
#endif //PACKET_SERIALIZER_USES_DEBUG_FOURCC

		if (nExpectedSize == SerializeBytes(convertor.m_Bytes, nConvertorSize, nFourCC))
		{
			if (IS_FALSE(SysMemory::IsSystemBigEndian()))
			{
				//-- little-endian system. Convert data from big-endian
				SysMemory::EndianSwap(convertor.m_Bytes, nConvertorSize);
			}

			nValue = convertor.m_nValue;
			return nExpectedSize;
		}
	}

	//-- Failed.
	if (Error::Ok == m_eError)
	{
		m_eError = Error::Fail;
	}
	return 0;
}


//----------------------------------------------------------//
// CPacketSerializer::SerializeS64
//----------------------------------------------------------//
size_t CPacketSerializer::SerializeS64(s64& nValue, u32 nFourCC)
{
	UBAs64 convertor;

	size_t nConvertorSize = sizeof(convertor);

	size_t nExpectedSize = nConvertorSize;
#if defined(PACKET_SERIALIZER_USES_DEBUG_FOURCC)
	nExpectedSize += sizeof(UBAFourCC);
#endif //PACKET_SERIALIZER_USES_DEBUG_FOURCC

	if (Mode::Serializing == m_eMode)
	{	
		convertor.m_nValue = nValue;
		
		//-- To be consistent with big-endian network traffic,
		//-- apply endian swap if system is little-endian.
		if (IS_FALSE(SysMemory::IsSystemBigEndian()))
		{
			//-- little-endian system. Convert data to big-endian
			SysMemory::EndianSwap(convertor.m_Bytes, nConvertorSize);
		}
		
		return SerializeBytes(convertor.m_Bytes, nConvertorSize, nFourCC);
	}
	else
	{
		size_t nExpectedSize = nConvertorSize;
#if defined(PACKET_SERIALIZER_USES_DEBUG_FOURCC)
		nExpectedSize += sizeof(UBAFourCC);
#endif //PACKET_SERIALIZER_USES_DEBUG_FOURCC

		if (nExpectedSize == SerializeBytes(convertor.m_Bytes, nConvertorSize, nFourCC))
		{
			if (IS_FALSE(SysMemory::IsSystemBigEndian()))
			{
				//-- little-endian system. Convert data from big-endian
				SysMemory::EndianSwap(convertor.m_Bytes, nConvertorSize);
			}

			nValue = convertor.m_nValue;
			return nExpectedSize;
		}
	}

	//-- Failed.
	if (Error::Ok == m_eError)
	{
		m_eError = Error::Fail;
	}
	return 0;
}


//----------------------------------------------------------//
// CPacketSerializer::SerializeU64
//----------------------------------------------------------//
size_t CPacketSerializer::SerializeU64(u64& nValue, u32 nFourCC)
{
	UBAu64 convertor;

	size_t nConvertorSize = sizeof(convertor);

	if (Mode::Serializing == m_eMode)
	{	
		convertor.m_nValue = nValue;
		
		//-- To be consistent with big-endian network traffic,
		//-- apply endian swap if system is little-endian.
		if (IS_FALSE(SysMemory::IsSystemBigEndian()))
		{
			//-- little-endian system. Convert data to big-endian
			SysMemory::EndianSwap(convertor.m_Bytes, nConvertorSize);
		}
		
		return SerializeBytes(convertor.m_Bytes, nConvertorSize, nFourCC);
	}
	else
	{
		size_t nExpectedSize = nConvertorSize;
#if defined(PACKET_SERIALIZER_USES_DEBUG_FOURCC)
		nExpectedSize += sizeof(UBAFourCC);
#endif //PACKET_SERIALIZER_USES_DEBUG_FOURCC

		if (nExpectedSize == SerializeBytes(convertor.m_Bytes, nConvertorSize, nFourCC))
		{
			if (IS_FALSE(SysMemory::IsSystemBigEndian()))
			{
				//-- little-endian system. Convert data from big-endian
				SysMemory::EndianSwap(convertor.m_Bytes, nConvertorSize);
			}

			nValue = convertor.m_nValue;
			return nExpectedSize;
		}
	}

	//-- Failed.
	if (Error::Ok == m_eError)
	{
		m_eError = Error::Fail;
	}
	return 0;
}


//----------------------------------------------------------//
// CPacketSerializer::SerializeBitfield
//----------------------------------------------------------//
size_t CPacketSerializer::SerializeBitfield(bitfield& nFlags, u32 nFourCC) 
{
	UBAbitfield convertor;

	size_t nConvertorSize = sizeof(convertor);

	if (Mode::Serializing == m_eMode)
	{	
		convertor.m_nValue = nFlags;
		
		//-- To be consistent with big-endian network traffic,
		//-- apply endian swap if system is little-endian.
		if (IS_FALSE(SysMemory::IsSystemBigEndian()))
		{
			//-- little-endian system. Convert data to big-endian
			SysMemory::EndianSwap(convertor.m_Bytes, nConvertorSize);
		}
		
		return SerializeBytes(convertor.m_Bytes, nConvertorSize, nFourCC);
	}
	else
	{
		size_t nExpectedSize = nConvertorSize;
#if defined(PACKET_SERIALIZER_USES_DEBUG_FOURCC)
		nExpectedSize += sizeof(UBAFourCC);
#endif //PACKET_SERIALIZER_USES_DEBUG_FOURCC

		if (nExpectedSize == SerializeBytes(convertor.m_Bytes, nConvertorSize, nFourCC))
		{
			if (IS_FALSE(SysMemory::IsSystemBigEndian()))
			{
				//-- little-endian system. Convert data from big-endian
				SysMemory::EndianSwap(convertor.m_Bytes, nConvertorSize);
			}

			nFlags = convertor.m_nValue;
			return nExpectedSize;
		}
	}

	//-- Failed.
	if (Error::Ok == m_eError)
	{
		m_eError = Error::Fail;
	}
	return 0;
}


//----------------------------------------------------------//
// CPacketSerializer::SerializeBool
//----------------------------------------------------------//
size_t CPacketSerializer::SerializeBool(bool& bValue, u32 nFourCC)
{
	u8 nByte;

	if (Mode::Serializing == m_eMode)
	{
		nByte = bValue ? 1 : 0;
		return SerializeU8(nByte, nFourCC);
	}
	else 
	{
		size_t nSize = SerializeU8(nByte, nFourCC);
		bValue = (nByte != 0);
		return nSize;
	}
}


//----------------------------------------------------------//
// CPacketSerializer::SerializeFixedString
//----------------------------------------------------------//
size_t CPacketSerializer::SerializeFixedString(FixedString& fixedString, u32 nFourCC)
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


//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//
