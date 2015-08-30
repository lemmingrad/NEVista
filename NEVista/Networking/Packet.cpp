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


//----------------------------------------------------------//
// DEFINES
//----------------------------------------------------------//

//----------------------------------------------------------//
// GLOBALS
//----------------------------------------------------------//

//----------------------------------------------------------//
// CPacket::CPacket
//----------------------------------------------------------//
CPacket::CPacket(SysString::Key key) 
: m_Key(key)
{
	SysMemory::Memclear(&m_HeaderV1, sizeof(m_HeaderV1));
	m_DataBuffer.Clear();
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
			return Error::Serializer;
		}

		//-- Serializer must have at least the size of the header in bytes free to proceed
		size_t nUnusedSize = serializer.GetSize() - serializer.GetOffset();
		if (nUnusedSize < GetHeaderSize())
		{
			//-- Not enough bytes in serializer.	
			return Error::Serializer;
		}

		//-- Version::V1
		if ( IS_ZERO(serializer.SerializeU8(m_HeaderV1.m_nFlags, 'flgs'))
			|| IS_ZERO(serializer.SerializeU16(m_HeaderV1.m_nMessages, 'mess')) )
		{
			return Error::Serializer;
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
				return Error::Serializer;
			}

			//-- We need a spare byte for null terminating the encoded base64 string.
			//-- This is blegh!
			//-- Test for unused space in serializer.
			size_t nUnusedSize = serializer.GetSize() - serializer.GetOffset();
			if (nUnusedSize < ((size_t)m_HeaderV1.m_nDataSize + 1))
			{
				//-- Not enough bytes in serializer.
				return Error::Serializer;
			}

			//-- Notice we do not include the extra byte when we reserve space.
			u8* pReserved = serializer.SerializeReserve(m_HeaderV1.m_nDataSize);
			if (IS_NULL_PTR(pReserved))
			{
				//-- Failed.
				return Error::Serializer;
			}

			//-- Valid encryption key?
			if (SysString::INVALID_KEY == m_Key)
			{
				return Error::EncryptionFailed;
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
				m_Key);

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
				return Error::Serializer;
			}

			u8* pReserved = serializer.SerializeReserve(m_HeaderV1.m_nDataSize);
			if (IS_NULL_PTR(pReserved))
			{
				//-- Failed.
				return Error::Serializer;
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

		//-- Read version number.
		if (IS_ZERO(serializer.SerializeU8(m_nVersion, 'pktV')))
		{
			//-- Failed to read version number.
			return Error::Serializer;
		}

		//-- Serializer must be at least the size of the header in bytes to proceed
		size_t nUnreadSize = serializer.GetSize() - serializer.GetOffset();
		if (nUnreadSize < GetHeaderSize())
		{
			//-- Not enough bytes in serializer.
			return Error::Serializer;
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
					return Error::Serializer;
				}

				if (Version::Unknown == Validate())
				{
					return Error::ProtocolMismatch;
				}

				u8* pDataSrc = serializer.SerializeReserve(m_HeaderV1.m_nDataSize);
				if (IS_NULL_PTR(pDataSrc))
				{
					//-- Failed.
					return Error::Serializer;
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
					
					//-- Valid encryption key?
					if (SysString::INVALID_KEY == m_Key)
					{
						return Error::EncryptionFailed;
					}

					nDecodedSize = SysString::KeyDecode(
						pDataDest, 
						nDecodedSize,
						(const s8*)pDataSrc, 
						m_HeaderV1.m_nDataSize,
						m_Key);

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
// CPacket::AddMessages
//----------------------------------------------------------//
CPacket::Error::Enum CPacket::AddMessages(TMessageList& sendList, TMessageList::iterator& it)
{
	bool bForcedEnd = false;
	bool bPacketIsCompressed = true;								//-- Prefer compressed.
	bool bPacketIsEncrypted = (SysString::INVALID_KEY != m_Key);	//-- Prefer encrypted if valid key is available.

	it = sendList.begin();
	
	u32 nMessages = 0;
	if (it != sendList.end())
	{
		SysSmartPtr<CMessage> first = *it;
		
		bPacketIsCompressed = first->CanBeCompressed(bPacketIsCompressed);
		if (IS_TRUE(bPacketIsCompressed))
		{
			SET_FLAG(m_HeaderV1.m_nFlags, Flag::Compressed);
		}

		bPacketIsEncrypted = first->CanBeEncrypted(bPacketIsEncrypted);
		if (IS_TRUE(bPacketIsEncrypted))
		{
			SET_FLAG(m_HeaderV1.m_nFlags, Flag::Encrypted);
		}
	}

	CPacketSerializer::Error::Enum eSerError = CPacketSerializer::Error::Ok;

	while ( IS_FALSE(bForcedEnd)
		&& (CPacketSerializer::Error::Ok == eSerError)
		&& (it != sendList.end()) )
	{
		SysSmartPtr<CMessage> message = *it;

		if (bPacketIsCompressed != message->CanBeCompressed(bPacketIsCompressed))
		{
			//-- Message cannot be included in same packet as previous message because
			//-- compressed flag has changed.
			bForcedEnd = true;
			break;
		}

		if (bPacketIsEncrypted != message->CanBeEncrypted(bPacketIsEncrypted))
		{
			//-- Message cannot be included in same packet as previous message because
			//-- encrypted flag has changed.
			bForcedEnd = true;
			break;
		}

		CPacketSerializer messageSerializer(CSerializer::Mode::Serializing, m_DataBuffer.Buffer() + m_DataBuffer.UsedSize(), m_DataBuffer.UnusedSize());
		message->Serialize(messageSerializer);

		eSerError = messageSerializer.GetError();
		switch (eSerError)
		{
			case CPacketSerializer::Error::Ok:
			{
				//-- Success
				if (IS_PTR(m_DataBuffer.InsTail(NULL, messageSerializer.GetOffset())))
				{
					++it;
					m_HeaderV1.m_nMessages++;

					if (IS_TRUE(message->IsForcedEnd()))
					{
						bForcedEnd = true;	
					}
				}
				else
				{
					//-- Error!
					return Error::BadFail;
				}
			}
			break;
			case CPacketSerializer::Error::EndReached:
			{
				//-- This just means we can break out of the loop.
				return Error::Ok;
			}
			break;
			default:
			{
				//-- Something bad went wrong.
				return Error::Serializer;
			}
			break;
		}
	}

	return Error::Ok;
}


//----------------------------------------------------------//
// CPacket::GetMessages
//----------------------------------------------------------//
CPacket::Error::Enum CPacket::GetMessages(TMessageList& tempList)
{
	//-- DataBuffer should have exactly m_HeaderV1.m_nMessages inside it.
	tempList.clear();

	u16 nProcessedMessages = 0;
	CPacketSerializer messageDeserializer(CSerializer::Mode::Deserializing, m_DataBuffer.Buffer(), m_DataBuffer.UsedSize());
	CPacketSerializer::Error::Enum eSerError = CPacketSerializer::Error::Ok;
	
	while ( (CPacketSerializer::Error::Ok == eSerError)
		&& (messageDeserializer.GetOffset() < messageDeserializer.GetSize()) )
	{
		CMessage::Type nMessageType = CMessage::kTypeUnknown;

		if (IS_ZERO(messageDeserializer.SerializeU32(nMessageType, 'type')))
		{
			return Error::Serializer;
		}

		CMessage* pMessage = CMessageFactory::CreateType(nMessageType);
		if (IS_NULL_PTR(pMessage))
		{
			return Error::ProtocolMismatch;
		}

		pMessage->Serialize(messageDeserializer);

		eSerError = messageDeserializer.GetError();
		if (CPacketSerializer::Error::Ok != eSerError)
		{
			return Error::Serializer;
		}

		SysSmartPtr<CMessage> smart(pMessage);
		tempList.push_back(smart);
		++nProcessedMessages;
	}

	//-- Sanity tests
	//-- messageSerializer should have consumed all its bytes.
	assert(messageDeserializer.GetOffset() == messageDeserializer.GetSize());
	if (messageDeserializer.GetOffset() != messageDeserializer.GetSize())
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
