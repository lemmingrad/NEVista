//----------------------------------------------------------//
// PACKETSERIALIZER.CPP
//----------------------------------------------------------//
//-- Description
// Serializer to stream data in and out of a byte buffer.
//----------------------------------------------------------//

#include "PacketSerializer.h"
#include "Types.h"
#include "SysMemory.h"
#include "Serializer.h"

//----------------------------------------------------------//
// DEFINES
//----------------------------------------------------------//

//----------------------------------------------------------//
// GLOBALS
//----------------------------------------------------------//

//----------------------------------------------------------//
// CPacketSerializer::CPacketSerializer
//----------------------------------------------------------//
CPacketSerializer::CPacketSerializer(Mode::Enum eMode, u8* pBuffer, size_t nSize, size_t nOffset)
: CSerializer(eMode)
, m_pBuffer(pBuffer)
, m_nSize(nSize)
, m_nOffset(nOffset)
, m_eError(Error::Ok)
{
}


//----------------------------------------------------------//
// CPacketSerializer::~CPacketSerializer
//----------------------------------------------------------//
CPacketSerializer::~CPacketSerializer()
{
}


//----------------------------------------------------------//
// CPacketSerializer::GetSize
//----------------------------------------------------------//
u8* CPacketSerializer::GetBuffer(void)
{
	return m_pBuffer;
}


//----------------------------------------------------------//
// CPacketSerializer::GetSize
//----------------------------------------------------------//
size_t CPacketSerializer::GetSize(void) const
{
	return m_nSize;
}


//----------------------------------------------------------//
// CPacketSerializer::GetOffset
//----------------------------------------------------------//
size_t CPacketSerializer::GetOffset(void) const
{
	return m_nOffset;
}


//----------------------------------------------------------//
// CPacketSerializer::GetError
//----------------------------------------------------------//
CPacketSerializer::Error::Enum CPacketSerializer::GetError(void) const
{
	return m_eError;
}


//----------------------------------------------------------//
// CPacketSerializer::SerializeReserve
//----------------------------------------------------------//
u8* CPacketSerializer::SerializeReserve(size_t nReservedSize)
{
	if (nReservedSize > 0)
	{
		size_t nRemaining = m_nSize - m_nOffset;
		if (nRemaining >= nReservedSize)
		{
			u8* pBuffer = &m_pBuffer[m_nOffset];
			if (m_eMode == Mode::Serializing)
			{
				SysMemory::Memclear(pBuffer, nReservedSize);
			}
			m_nOffset += nReservedSize;
			return pBuffer;
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
	return NULL;
}


//----------------------------------------------------------//
// CPacketSerializer::SerializeBytes
//----------------------------------------------------------//
// Serialize in/out a number of bytes.
// Also does debug FourCC if required.
//----------------------------------------------------------//
size_t CPacketSerializer::SerializeBytes(u8* pData, size_t nDataSize, u32 nFourCC)
{
	if ( IS_PTR(pData)
		&& (nDataSize > 0) 
		&& IS_PTR(m_pBuffer) )
	{
		size_t nRemaining = m_nSize - m_nOffset;
		size_t nRequired = nDataSize;

#if defined(PACKET_SERIALIZER_USES_DEBUG_FOURCC)
		UBAFourCC FCC;
		size_t nFourCCSize = sizeof(UBAFourCC);
		nRequired += nFourCCSize;
#endif //PACKET_SERIALIZER_USES_DEBUG_FOURCC

		if (nRemaining >= nRequired)
		{
			u8* pBuffer = &m_pBuffer[m_nOffset];

			if (m_eMode == Mode::Serializing)
			{
#if defined(PACKET_SERIALIZER_USES_DEBUG_FOURCC)
				FCC.m_nValue = nFourCC;
		
				//-- To be consistent with big-endian network traffic,
				//-- apply endian swap if system is little-endian.
				if (IS_FALSE(SysMemory::IsSystemBigEndian()))
				{
					//-- little-endian system. Convert data to big-endian
					SysMemory::EndianSwap(FCC.m_Bytes, nFourCCSize);
				}	

				if (IS_PTR(SysMemory::Memcpy(pBuffer, nRemaining, FCC.m_Bytes, nFourCCSize)))
				{
					m_nOffset += nFourCCSize;
					nRemaining -= nFourCCSize;
					pBuffer += nFourCCSize;
				}
				else
				{
					m_eError = Error::CopyFailed;
					return 0;
				}
#endif //PACKET_SERIALIZER_USES_DEBUG_FOURCC

				//-- Mode::Serializing, input variables to buffer.
				if (IS_PTR(SysMemory::Memcpy(pBuffer, nRemaining, pData, nDataSize)))
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
			else
			{
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
		
	
//----------------------------------------------------------//
// CPacketSerializer::SerializeF32
//----------------------------------------------------------//
size_t CPacketSerializer::SerializeF32(f32& fValue, u32 nFourCC)
{
	UBAf32 convertor;

	size_t nConvertorSize = sizeof(convertor);

	if (m_eMode == Mode::Serializing)
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
	return 0;
}


//----------------------------------------------------------//
// CPacketSerializer::SerializeF64
//----------------------------------------------------------//
size_t CPacketSerializer::SerializeF64(f64& fValue, u32 nFourCC)
{
	UBAf64 convertor;

	size_t nConvertorSize = sizeof(convertor);

	if (m_eMode == Mode::Serializing)
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
	return 0;
}


//----------------------------------------------------------//
// CPacketSerializer::SerializeS32
//----------------------------------------------------------//
size_t CPacketSerializer::SerializeS32(s32& nValue, u32 nFourCC)
{
	UBAs32 convertor;

	size_t nConvertorSize = sizeof(convertor);

	if (m_eMode == Mode::Serializing)
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
	return 0;
}


//----------------------------------------------------------//
// CPacketSerializer::SerializeU32
//----------------------------------------------------------//
size_t CPacketSerializer::SerializeU32(u32& nValue, u32 nFourCC)
{
	UBAu32 convertor;

	size_t nConvertorSize = sizeof(convertor);

	if (m_eMode == Mode::Serializing)
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
	return 0;
}


//----------------------------------------------------------//
// CPacketSerializer::SerializeS8
//----------------------------------------------------------//
size_t CPacketSerializer::SerializeS8(s8& nValue, u32 nFourCC) 
{
	u8* pByte = (u8*)&nValue;

	size_t nConvertorSize = sizeof(u8);

	if (m_eMode == Mode::Serializing)
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
	return 0;
}


//----------------------------------------------------------//
// CPacketSerializer::SerializeU8
//----------------------------------------------------------//
size_t CPacketSerializer::SerializeU8(u8& nValue, u32 nFourCC)
{
	u8* pByte = (u8*)&nValue;

	size_t nConvertorSize = sizeof(u8);

	if (m_eMode == Mode::Serializing)
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
	return 0;
}


//----------------------------------------------------------//
// CPacketSerializer::SerializeS16
//----------------------------------------------------------//
size_t CPacketSerializer::SerializeS16(s16& nValue, u32 nFourCC) 
{
	UBAs16 convertor;
	
	size_t nConvertorSize = sizeof(convertor);

	if (m_eMode == Mode::Serializing)
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
	return 0;
}


//----------------------------------------------------------//
// CPacketSerializer::SerializeU16
//----------------------------------------------------------//
size_t CPacketSerializer::SerializeU16(u16& nValue, u32 nFourCC) 
{
	UBAu16 convertor;

	size_t nConvertorSize = sizeof(convertor);

	if (m_eMode == Mode::Serializing)
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

	if (m_eMode == Mode::Serializing)
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
	return 0;
}


//----------------------------------------------------------//
// CPacketSerializer::SerializeU64
//----------------------------------------------------------//
size_t CPacketSerializer::SerializeU64(u64& nValue, u32 nFourCC)
{
	UBAu64 convertor;

	size_t nConvertorSize = sizeof(convertor);

	if (m_eMode == Mode::Serializing)
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
	return 0;
}


//----------------------------------------------------------//
// CPacketSerializer::SerializeBitfield
//----------------------------------------------------------//
size_t CPacketSerializer::SerializeBitfield(bitfield& nFlags, u32 nFourCC) 
{
	UBAbitfield convertor;

	size_t nConvertorSize = sizeof(convertor);

	if (m_eMode == Mode::Serializing)
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
	return 0;
}


//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//
