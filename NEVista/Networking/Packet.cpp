//----------------------------------------------------------//
// PACKET.CPP
//----------------------------------------------------------//
//-- Description			
// A packet class. 
// Groups of messages are sent across the connection in a 
// packet. Each packet may or may not be encrypted and/or
// compressed. Packets are guaranteed to contain at least
// one message, and all messages must be complete.
//----------------------------------------------------------//


#include "Packet.h"
#include "Types.h"
#include "SysSmartPtr.h"
#include "SysString.h"
#include "Serialized.h"
#include "PacketSerializer.h"
#include "Message.h"

#include "Messages/MsgMotd.h"


//----------------------------------------------------------//
// DEFINES
//----------------------------------------------------------//

//----------------------------------------------------------//
// GLOBALS
//----------------------------------------------------------//

//----------------------------------------------------------//
// CPacket::CPacket
//----------------------------------------------------------//
CPacket::CPacket(u8 nFlags) 
{
	SysMemory::Memclear(&m_HeaderV1, sizeof(m_HeaderV1));

	//-- Version::V1
	if (TEST_FLAG(nFlags, Flag::Encrypted))
	{
		SET_FLAG(m_HeaderV1.m_nFlags, Flag::Encrypted);
	}
	if (TEST_FLAG(nFlags, Flag::Compressed))
	{
		SET_FLAG(m_HeaderV1.m_nFlags, Flag::Compressed);
	}

	m_DataBuffer.Clear();
	m_MessageList.clear();
}


//----------------------------------------------------------//
// CPacket::~CPacket
//----------------------------------------------------------//
CPacket::~CPacket() 
{
}


//----------------------------------------------------------//
// CPacket::GetMinimumReadSize
//----------------------------------------------------------//
//-- Description
// Returns the minimum required size needed before we can
// start parsing a packet header.
//----------------------------------------------------------//
size_t CPacket::GetMinimumReadSize(void)
{
	//-- Could be just the version number (1 byte).
	//-- But seems pointless when we only have 1 header version at the momemnt.
	//-- So let's include the sizeof(HeaderV1) in the minimum read size too.
	return sizeof(u8) + sizeof(HeaderV1);
}


//----------------------------------------------------------//
// CPacket::GetHeaderSize
//----------------------------------------------------------//
//-- Description
// Returns the size of a packet header based on version.
//----------------------------------------------------------//
size_t CPacket::GetHeaderSize(void) const
{
	switch (m_nVersion)
	{
		case Version::V1:
		{
			return sizeof(HeaderV1);
		}
		break;

		default:
		break;
	}
	
	return 0;
}


//----------------------------------------------------------//
// CPacket::Validate
//----------------------------------------------------------//
//-- Description
// Basic valid data test on header. Returns current header 
// version if header is valid.
//----------------------------------------------------------//
CPacket::Version::Enum CPacket::Validate(void) const
{
	switch (m_nVersion)
	{
		case Version::V1:
		{
			if ( (GetDataSize() > 0)
				&& (GetMessageCount() > 0) )
			{
				return (Version::Enum)m_nVersion;
			}
		}
		break;

		default:
		break;
	}

	return Version::Unknown;
}


//----------------------------------------------------------//
// CPacket::IsEncrypted
//----------------------------------------------------------//
bool CPacket::IsEncrypted(void) const
{
	switch (m_nVersion)
	{
		case Version::V1:
		{
			return TEST_FLAG(m_HeaderV1.m_nFlags, Flag::Encrypted);
		}
		break;

		default:
		break;
	}

	return false;
}


//----------------------------------------------------------//
// CPacket::IsCompressed
//----------------------------------------------------------//
bool CPacket::IsCompressed(void) const
{
	switch (m_nVersion)
	{
		case Version::V1:
		{
			return TEST_FLAG(m_HeaderV1.m_nFlags, Flag::Compressed);
		}
		break;

		default:
		break;
	}

	return false;
}


//----------------------------------------------------------//
// CPacket::GetMessageCount
//----------------------------------------------------------//
u16 CPacket::GetMessageCount(void) const
{
	switch (m_nVersion)
	{
		case Version::V1:
		{
			return m_HeaderV1.m_nMessages;
		}
		break;

		default:
		break;
	}

	return 0;
}


//----------------------------------------------------------//
// CPacket::GetDataSize
//----------------------------------------------------------//
size_t CPacket::GetDataSize(void) const
{
	switch (m_nVersion)
	{
		case Version::V1:
		{
			return m_HeaderV1.m_nDataSize;
		}
		break;

		default:
		break;
	}

	return 0;
}


//----------------------------------------------------------//
// CPacket::Serialize
//----------------------------------------------------------//
CPacket::Error::Enum CPacket::Serialize(CPacketSerializer& serializer)
{
	if (CSerializer::Mode::Serializing == serializer.GetMode())
	{
		//-- Serializing
		m_nVersion = Version::Current;

		if (IS_ZERO(serializer.SerializeU8(m_nVersion, 'pktV')))
		{
			return Error::SerializerFull;
		}

		//-- Serializer must have at least the size of the header in bytes free to proceed
		size_t nUnusedSize = serializer.GetSize() - serializer.GetOffset();
		if (nUnusedSize < GetHeaderSize())
		{
			//-- Not enough bytes in serializer.
			return Error::SerializerFull;
		}

		//-- Version::V1
		if ( IS_ZERO(serializer.SerializeU8(m_HeaderV1.m_nFlags, 'flgs'))
			|| IS_ZERO(serializer.SerializeU16(m_HeaderV1.m_nMessages, 'mess')) )
		{
			return Error::SerializerFull;
		}
	
		if (IS_TRUE(IsEncrypted()))
		{
			size_t nEncodedSize = SysString::Base64EncodedSize(m_DataBuffer.UsedSize());
			if (IS_ZERO(nEncodedSize))
			{
				//-- Failed.
				//-- Something went wrong with the Encode.
				return Error::EncryptionFailed;
			}

			m_HeaderV1.m_nDataSize = nEncodedSize;

			if (IS_ZERO(serializer.SerializeU16(m_HeaderV1.m_nDataSize, 'size')))
			{
				return Error::SerializerFull;
			}

			//-- We need a spare byte for null terminating the encoded base64 string.
			//-- This is blegh!
			//-- Test for unused space in serializer.
			size_t nUnusedSize = serializer.GetSize() - serializer.GetOffset();
			if (nUnusedSize < ((size_t)m_HeaderV1.m_nDataSize + 1))
			{
				//-- Not enough bytes in serializer.
				return Error::SerializerFull;
			}

			//-- Notice we do not include the extra byte when we reserve space.
			u8* pReserved = serializer.SerializeReserve(m_HeaderV1.m_nDataSize);
			if (IS_NULL_PTR(pReserved))
			{
				//-- Failed.
				return Error::SerializerFull;
			}

			//-- NOTE: this will actually put a null terminator beyond the end 
			//-- of the pReserved block! But should be ok because we checked 
			//-- if there was room enough in the unused size of the serializer
			//-- for it.
			//-- Whatever gets serialized next will overwrite the null terminator,
			//-- which is ok.
			nEncodedSize = SysString::KeyEncode(
				(s8*)pReserved, 
				m_HeaderV1.m_nDataSize + 1,				
				m_DataBuffer.Buffer(), 
				m_DataBuffer.UsedSize(),
				SysString::GenerateKey(0, 0));

			if (IS_ZERO(nEncodedSize))
			{
				//-- Failed.
				//-- Something went wrong with the Encode.
				return Error::EncryptionFailed;
			}

			if (IS_NULL_PTR(m_DataBuffer.StripHead(m_DataBuffer.UsedSize())))
			{
				//-- Failed.
				//-- DataBuffer wasn't full enough
				return Error::DataBufferEmpty;
			}
		}
		else
		{
			//-- Write directly into serializer.
			m_HeaderV1.m_nDataSize = m_DataBuffer.UsedSize();

			if (IS_ZERO(serializer.SerializeU16(m_HeaderV1.m_nDataSize, 'size')))
			{
				return Error::SerializerFull;
			}

			u8* pReserved = serializer.SerializeReserve(m_HeaderV1.m_nDataSize);
			if (IS_NULL_PTR(pReserved))
			{
				//-- Failed.
				return Error::SerializerFull;
			}

			if (IS_NULL_PTR(SysMemory::Memcpy(pReserved, m_HeaderV1.m_nDataSize, m_DataBuffer.Buffer(), m_HeaderV1.m_nDataSize)))
			{
				//-- Failed.
				return Error::CopyFailed;
			}

			if (IS_NULL_PTR(m_DataBuffer.StripHead(m_HeaderV1.m_nDataSize)))
			{
				//-- Failed.
				//-- DataBuffer wasn't full enough
				return Error::DataBufferEmpty;
			}
		}

		//-- DataBuffer should now be empty.
		assert(IS_ZERO(m_DataBuffer.UsedSize()));
		assert(m_HeaderV1.m_nMessages > 0);
		assert(m_HeaderV1.m_nDataSize > 0);
		//-- End.
	}
	else
	{
		//-- Deserializing.
		SysMemory::Memclear(&m_HeaderV1, sizeof(m_HeaderV1));
		m_DataBuffer.Clear();
		m_MessageList.clear();

		//-- Read version number.
		if (IS_ZERO(serializer.SerializeU8(m_nVersion, 'pktV')))
		{
			//-- Failed to read version number.
			return Error::SerializerEmpty;
		}

		//-- Serializer must be at least the size of the header in bytes to proceed
		size_t nUnreadSize = serializer.GetSize() - serializer.GetOffset();
		if (nUnreadSize < GetHeaderSize())
		{
			//-- Not enough bytes in serializer.
			return Error::SerializerEmpty;
		}

		//-- Read the header
		switch (m_nVersion)
		{
			case Version::V1:
			{
				if ( IS_ZERO(serializer.SerializeU8(m_HeaderV1.m_nFlags, 'flgs'))
					|| IS_ZERO(serializer.SerializeU16(m_HeaderV1.m_nMessages, 'mess'))
					|| IS_ZERO(serializer.SerializeU16(m_HeaderV1.m_nDataSize, 'size')) )
				{
					return Error::SerializerEmpty;
				}

				if (Version::Unknown == Validate())
				{
					return Error::ProtocolMismatch;
				}

				u8* pDataSrc = serializer.SerializeReserve(m_HeaderV1.m_nDataSize);
				if (IS_NULL_PTR(pDataSrc))
				{
					//-- Failed.
					return Error::SerializerEmpty;
				}

				//-- Is packet encrypted?
				if (IS_TRUE(IsEncrypted()))
				{
					//-- Decrypt packet payload data to DataBuffer
					size_t nDecodedSize = SysString::Base64DecodedSize((const s8*)pDataSrc, m_HeaderV1.m_nDataSize);
					if (IS_ZERO(nDecodedSize))
					{
						//-- Failed.
						//-- Something went wrong with the Decode.
						return Error::EncryptionFailed;
					}
					assert(m_DataBuffer.Size() >= nDecodedSize);

					u8* pDataDest = m_DataBuffer.InsTail(NULL, nDecodedSize);
					if (IS_NULL_PTR(pDataDest))
					{
						//-- Failed.
						//-- DataBuffer is too full.
						return Error::DataBufferFull;
					}
					
					nDecodedSize = SysString::KeyDecode(
						pDataDest, 
						nDecodedSize,
						(const s8*)pDataSrc, 
						m_HeaderV1.m_nDataSize,
						SysString::GenerateKey(0, 0));

					if (IS_ZERO(nDecodedSize))
					{
						//-- Failed.
						//-- Something went wrong with the Decode.
						return Error::EncryptionFailed;
					}
				}
				else
				{
					//-- Read directly into buffer.
					assert(m_DataBuffer.Size() >= m_HeaderV1.m_nDataSize);

					if (IS_NULL_PTR(m_DataBuffer.InsTail(pDataSrc, m_HeaderV1.m_nDataSize)))
					{
						//-- Failed.
						//-- DataBuffer is too full.
						return Error::DataBufferFull;
					}
				}

				//-- DataBuffer should now have exactly m_HeaderV1.m_nMessages inside it.
				//-- End.
			}
			break;
			default:
			{
				//-- Unexpected version
				return Error::UnknownVersion;
			}
			break;
		}
	}

	return Error::Ok;
}


//----------------------------------------------------------//
// CPacket::AddMessage
//----------------------------------------------------------//
CPacket::Error::Enum CPacket::AddMessage(SysSmartPtr<CMessage> message)
{
	CPacketSerializer messageSerializer(CSerializer::Mode::Serializing, m_DataBuffer.Buffer() + m_DataBuffer.UsedSize(), m_DataBuffer.UnusedSize());

	message->Serialize(messageSerializer);

	CPacketSerializer::Error::Enum eErrorCode = messageSerializer.GetError();
	if (CPacketSerializer::Error::Ok == eErrorCode)
	{
		//-- Success
		m_DataBuffer.InsTail(NULL, messageSerializer.GetOffset());
		m_HeaderV1.m_nMessages++;
	}
	else
	{
		//-- Failed.
		if (CPacketSerializer::Error::EndReached == eErrorCode)
		{
			return Error::DataBufferFull;
		}
		else
		{
			return Error::CopyFailed;
		}
	}

	return Error::Ok;
}


//----------------------------------------------------------//
// CPacket::ConvertToMessages
//----------------------------------------------------------//
CPacket::Error::Enum CPacket::ConvertToMessages(TMessageList& pushList)
{
	//-- DataBuffer should have exactly m_HeaderV1.m_nMessages inside it.

	m_MessageList.clear();
	
	u16 nProcessedMessages = 0;
	CPacketSerializer messageSerializer(CSerializer::Mode::Deserializing, m_DataBuffer.Buffer(), m_DataBuffer.UsedSize());
	CPacketSerializer::Error::Enum eErrorCode = CPacketSerializer::Error::Ok;
	
	while ( (CPacketSerializer::Error::Ok == eErrorCode)
		&& (messageSerializer.GetOffset() < messageSerializer.GetSize()) )
	{
		u32 nMessageType = CMessage::Type::Unknown;

		if (IS_ZERO(messageSerializer.SerializeU32(nMessageType, 'type')))
		{
			return Error::ProtocolMismatch;
		}

		CMessage* pMessage = CMessage::CreateType((CMessage::Type::Enum)nMessageType);
		if (IS_NULL_PTR(pMessage))
		{
			return Error::ProtocolMismatch;
		}

		pMessage->Serialize(messageSerializer);

		eErrorCode = messageSerializer.GetError();
		if (CPacketSerializer::Error::Ok != eErrorCode)
		{
			if (CPacketSerializer::Error::EndReached == eErrorCode)
			{
				return Error::DataBufferEmpty;
			}
			else
			{
				return Error::ProtocolMismatch;
			}
		}

		SysSmartPtr<CMessage> smart(pMessage);
		m_MessageList.push_back(smart);
		++nProcessedMessages;
	}

	//-- Sanity tests
	//-- messageSerializer should have consumed all its bytes.
	assert(messageSerializer.GetOffset() == messageSerializer.GetSize());
	if (messageSerializer.GetOffset() != messageSerializer.GetSize())
	{
		return Error::SanityFail;
	}

	//-- We should have processed exactly the right number of messages.
	assert(nProcessedMessages == GetMessageCount());
	if (nProcessedMessages != GetMessageCount())
	{
		return Error::SanityFail;
	}

	return Error::Ok;
}


//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//
