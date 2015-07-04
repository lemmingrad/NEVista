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
{
};


//----------------------------------------------------------//
// CPacketSerializer::~CPacketSerializer
//----------------------------------------------------------//
CPacketSerializer::~CPacketSerializer()
{
}


//----------------------------------------------------------//
// CPacketSerializer::GetOffset
//----------------------------------------------------------//
size_t CPacketSerializer::GetOffset(void)
{
	return m_nOffset;
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
		size_t nFourCCSize = sizeof(u32);
		
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
#endif //PACKET_SERIALIZER_USES_DEBUG_FOURCC

				//-- Mode::Serializing, input variables to buffer.
				if (IS_PTR(SysMemory::Memcpy(pBuffer, nRemaining, pData, nDataSize)))
				{
					m_nOffset += nDataSize;
					return nDataSize;
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
					}
				}
#endif //PACKET_SERIALIZER_USES_DEBUG_FOURCC

				//-- Mode::Deserializing, buffer to output variables.
				if (IS_PTR(SysMemory::Memcpy(pData, nDataSize, pBuffer, nDataSize)))
				{
					m_nOffset += nDataSize;
					return nDataSize;
				}
			}
		}
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
	size_t nConvertSize = sizeof(f32);

	if (m_eMode == Mode::Serializing)
	{	
		convertor.m_fValue = fValue;
		
		//-- To be consistent with big-endian network traffic,
		//-- apply endian swap if system is little-endian.
		if (IS_FALSE(SysMemory::IsSystemBigEndian()))
		{
			//-- little-endian system. Convert data to big-endian
			SysMemory::EndianSwap(convertor.m_Bytes, nConvertSize);
		}
		
		return SerializeBytes(convertor.m_Bytes, nConvertSize, nFourCC);
	}
	else
	{
		if (nConvertSize == SerializeBytes(convertor.m_Bytes, nConvertSize, nFourCC))
		{
			if (IS_FALSE(SysMemory::IsSystemBigEndian()))
			{
				//-- little-endian system. Convert data from big-endian
				SysMemory::EndianSwap(convertor.m_Bytes, nConvertSize);
			}

			fValue = convertor.m_fValue;
			return nConvertSize;
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
	size_t nConvertSize = sizeof(f64);

	if (m_eMode == Mode::Serializing)
	{	
		convertor.m_fValue = fValue;
		
		//-- To be consistent with big-endian network traffic,
		//-- apply endian swap if system is little-endian.
		if (IS_FALSE(SysMemory::IsSystemBigEndian()))
		{
			//-- little-endian system. Convert data to big-endian
			SysMemory::EndianSwap(convertor.m_Bytes, nConvertSize);
		}
		
		return SerializeBytes(convertor.m_Bytes, nConvertSize, nFourCC);
	}
	else
	{
		if (nConvertSize == SerializeBytes(convertor.m_Bytes, nConvertSize, nFourCC))
		{
			if (IS_FALSE(SysMemory::IsSystemBigEndian()))
			{
				//-- little-endian system. Convert data from big-endian
				SysMemory::EndianSwap(convertor.m_Bytes, nConvertSize);
			}

			fValue = convertor.m_fValue;
			return nConvertSize;
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
	size_t nConvertSize = sizeof(s32);

	if (m_eMode == Mode::Serializing)
	{	
		convertor.m_nValue = nValue;
		
		//-- To be consistent with big-endian network traffic,
		//-- apply endian swap if system is little-endian.
		if (IS_FALSE(SysMemory::IsSystemBigEndian()))
		{
			//-- little-endian system. Convert data to big-endian
			SysMemory::EndianSwap(convertor.m_Bytes, nConvertSize);
		}
		
		return SerializeBytes(convertor.m_Bytes, nConvertSize, nFourCC);
	}
	else
	{
		if (nConvertSize == SerializeBytes(convertor.m_Bytes, nConvertSize, nFourCC))
		{
			if (IS_FALSE(SysMemory::IsSystemBigEndian()))
			{
				//-- little-endian system. Convert data from big-endian
				SysMemory::EndianSwap(convertor.m_Bytes, nConvertSize);
			}

			nValue = convertor.m_nValue;
			return nConvertSize;
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
	size_t nConvertSize = sizeof(u32);

	if (m_eMode == Mode::Serializing)
	{	
		convertor.m_nValue = nValue;
		
		//-- To be consistent with big-endian network traffic,
		//-- apply endian swap if system is little-endian.
		if (IS_FALSE(SysMemory::IsSystemBigEndian()))
		{
			//-- little-endian system. Convert data to big-endian
			SysMemory::EndianSwap(convertor.m_Bytes, nConvertSize);
		}
		
		return SerializeBytes(convertor.m_Bytes, nConvertSize, nFourCC);
	}
	else
	{
		if (nConvertSize == SerializeBytes(convertor.m_Bytes, nConvertSize, nFourCC))
		{
			if (IS_FALSE(SysMemory::IsSystemBigEndian()))
			{
				//-- little-endian system. Convert data from big-endian
				SysMemory::EndianSwap(convertor.m_Bytes, nConvertSize);
			}

			nValue = convertor.m_nValue;
			return nConvertSize;
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
	size_t nConvertSize = sizeof(s8);

	return SerializeBytes(pByte, nConvertSize, nFourCC);
}


//----------------------------------------------------------//
// CPacketSerializer::SerializeU8
//----------------------------------------------------------//
size_t CPacketSerializer::SerializeU8(u8& nValue, u32 nFourCC)
{
	u8* pByte = (u8*)&nValue;
	size_t nConvertSize = sizeof(u8);

	return SerializeBytes(pByte, nConvertSize, nFourCC);
}


//----------------------------------------------------------//
// CPacketSerializer::SerializeS16
//----------------------------------------------------------//
size_t CPacketSerializer::SerializeS16(s16& nValue, u32 nFourCC) 
{
	UBAs16 convertor;
	size_t nConvertSize = sizeof(s16);

	if (m_eMode == Mode::Serializing)
	{	
		convertor.m_nValue = nValue;
		
		//-- To be consistent with big-endian network traffic,
		//-- apply endian swap if system is little-endian.
		if (IS_FALSE(SysMemory::IsSystemBigEndian()))
		{
			//-- little-endian system. Convert data to big-endian
			SysMemory::EndianSwap(convertor.m_Bytes, nConvertSize);
		}
		
		return SerializeBytes(convertor.m_Bytes, nConvertSize, nFourCC);
	}
	else
	{
		if (nConvertSize == SerializeBytes(convertor.m_Bytes, nConvertSize, nFourCC))
		{
			if (IS_FALSE(SysMemory::IsSystemBigEndian()))
			{
				//-- little-endian system. Convert data from big-endian
				SysMemory::EndianSwap(convertor.m_Bytes, nConvertSize);
			}

			nValue = convertor.m_nValue;
			return nConvertSize;
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
	size_t nConvertSize = sizeof(u16);

	if (m_eMode == Mode::Serializing)
	{	
		convertor.m_nValue = nValue;
		
		//-- To be consistent with big-endian network traffic,
		//-- apply endian swap if system is little-endian.
		if (IS_FALSE(SysMemory::IsSystemBigEndian()))
		{
			//-- little-endian system. Convert data to big-endian
			SysMemory::EndianSwap(convertor.m_Bytes, nConvertSize);
		}
		
		return SerializeBytes(convertor.m_Bytes, nConvertSize, nFourCC);
	}
	else
	{
		if (nConvertSize == SerializeBytes(convertor.m_Bytes, nConvertSize, nFourCC))
		{
			if (IS_FALSE(SysMemory::IsSystemBigEndian()))
			{
				//-- little-endian system. Convert data from big-endian
				SysMemory::EndianSwap(convertor.m_Bytes, nConvertSize);
			}

			nValue = convertor.m_nValue;
			return nConvertSize;
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
	size_t nConvertSize = sizeof(s64);

	if (m_eMode == Mode::Serializing)
	{	
		convertor.m_nValue = nValue;
		
		//-- To be consistent with big-endian network traffic,
		//-- apply endian swap if system is little-endian.
		if (IS_FALSE(SysMemory::IsSystemBigEndian()))
		{
			//-- little-endian system. Convert data to big-endian
			SysMemory::EndianSwap(convertor.m_Bytes, nConvertSize);
		}
		
		return SerializeBytes(convertor.m_Bytes, nConvertSize, nFourCC);
	}
	else
	{
		if (nConvertSize == SerializeBytes(convertor.m_Bytes, nConvertSize, nFourCC))
		{
			if (IS_FALSE(SysMemory::IsSystemBigEndian()))
			{
				//-- little-endian system. Convert data from big-endian
				SysMemory::EndianSwap(convertor.m_Bytes, nConvertSize);
			}

			nValue = convertor.m_nValue;
			return nConvertSize;
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
	size_t nConvertSize = sizeof(u64);

	if (m_eMode == Mode::Serializing)
	{	
		convertor.m_nValue = nValue;
		
		//-- To be consistent with big-endian network traffic,
		//-- apply endian swap if system is little-endian.
		if (IS_FALSE(SysMemory::IsSystemBigEndian()))
		{
			//-- little-endian system. Convert data to big-endian
			SysMemory::EndianSwap(convertor.m_Bytes, nConvertSize);
		}
		
		return SerializeBytes(convertor.m_Bytes, nConvertSize, nFourCC);
	}
	else
	{
		if (nConvertSize == SerializeBytes(convertor.m_Bytes, nConvertSize, nFourCC))
		{
			if (IS_FALSE(SysMemory::IsSystemBigEndian()))
			{
				//-- little-endian system. Convert data from big-endian
				SysMemory::EndianSwap(convertor.m_Bytes, nConvertSize);
			}

			nValue = convertor.m_nValue;
			return nConvertSize;
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
	size_t nConvertSize = sizeof(bitfield);

	if (m_eMode == Mode::Serializing)
	{	
		convertor.m_nValue = nFlags;
		
		//-- To be consistent with big-endian network traffic,
		//-- apply endian swap if system is little-endian.
		if (IS_FALSE(SysMemory::IsSystemBigEndian()))
		{
			//-- little-endian system. Convert data to big-endian
			SysMemory::EndianSwap(convertor.m_Bytes, nConvertSize);
		}
		
		return SerializeBytes(convertor.m_Bytes, nConvertSize, nFourCC);
	}
	else
	{
		if (nConvertSize == SerializeBytes(convertor.m_Bytes, nConvertSize, nFourCC))
		{
			if (IS_FALSE(SysMemory::IsSystemBigEndian()))
			{
				//-- little-endian system. Convert data from big-endian
				SysMemory::EndianSwap(convertor.m_Bytes, nConvertSize);
			}

			nFlags = convertor.m_nValue;
			return nConvertSize;
		}
	}

	//-- Failed.
	return 0;
}


//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//
