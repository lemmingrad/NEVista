


#include "TCPConnection.h"
#include "Types.h"
#include "Packet.h"
#include "PacketSerializer.h"
#include "SimpleBuffer.h"



CTCPConnection::CTCPConnection()
: m_eState(State::Unknown)
, m_nRecvMessages(0)
, m_nSendMessages(0)
, m_MessageRecvBuffer()
, m_MessageSendBuffer()
, m_PacketRecvBuffer()
, m_PacketSendBuffer()
{
}

CTCPConnection::~CTCPConnection()
{
}

CTCPConnection::Error::Enum CTCPConnection::Initialise(void)
{
	m_eState = State::Closed;

	return Error::Ok;
}

CTCPConnection::Error::Enum CTCPConnection::Shutdown(void)
{
	return Error::Ok;
}

CTCPConnection::Error::Enum CTCPConnection::Update(void)
{
	Error::Enum eRet = Error::Ok;
	
	eRet = UpdateRecv();
	switch (eRet)
	{
		case Error::UnknownVersion:	
		case Error::ProtocolMismatch:
		case Error::SanityFail:					
		case Error::BadFail:
		{
			//-- Bad fail. This is fatal and should trigger a connection reset.
			return eRet;
		}
		break;
		default:
			//-- Ok and not so bad fails may continue.
		break;
	}

//	eRet = UpdateProcess();
//	if (Error::Fail == eRet)
//	{
//		return eRet;
//	}

//	eRet = UpdateSend();
//	if (Error::Fail == eRet)
//	{
//		return eRet;
//	}

	return eRet;
}

CTCPConnection::Error::Enum CTCPConnection::UpdateRecv(void)
{
	//-- Check for TCP received data.
	//-- If enough data is present, decrypt and copy received messages/contents as necessary.

	CPacketSerializer serializer(CSerializer::Mode::Deserializing, m_PacketRecvBuffer.Buffer(), m_PacketRecvBuffer.UsedSize(), 0);

	while (m_PacketRecvBuffer.UsedSize() >= CPacketHeader::GetMaxHeaderSize())
	{
		//-- Enough bytes available to fill in a header.

		Error::Enum eError = Error::Ok;

		CPacketHeader header;
		header.Serialize(serializer);

		switch (header.Validate())
		{
			case CPacketHeader::Version::V1:
			{
				eError = UpdateRecvV1(header);
				if (Error::Ok != eError)
				{
					//-- Failed. 
					//-- Might be a bad fail, or it might just be a full or empty buffer
					//-- we can re-test next time.
					return eError;
				}
			}
			break;

			case CPacketHeader::Version::Unknown:
			default:
			{
				//-- Bad fail.
				return Error::UnknownVersion;
			}
			break;
		}
	}

	//-- Sanity tests
	if ( (m_nRecvMessages > 0) 
		&& (IS_ZERO(m_MessageRecvBuffer.UsedSize())) )
	{
		//-- Mismatch. MessageRecvBuffer is empty, but number of messages greater than 0.
		return Error::SanityFail;
	}
	if ( IS_ZERO(m_nRecvMessages) 
		&& (m_MessageRecvBuffer.UsedSize() > 0) )
	{
		//-- Mismatch. Number of messages is 0, but MessageRecvBuffer is not empty.
		return Error::SanityFail;
	}

	return Error::Ok;
}


CTCPConnection::Error::Enum CTCPConnection::UpdateRecvV1(CPacketHeader& header)
{
	size_t nRequiredBytes = header.GetHeaderSize() + header.GetDataSize();

	if (m_PacketRecvBuffer.UsedSize() < nRequiredBytes)
	{
		//-- Failed.
		//-- Not enough bytes in PacketRecvBuffer to process the full packet.
		return Error::PacketRecvBufferEmpty;
	}

	//-- Enough bytes available to process the full packet	
	//-- Copy data to message queue (with possible decrypt)
	if (IS_TRUE(header.IsEncrypted()))
	{
		//-- Reserve some space in MessageRecvBuffer.
		//-- Note that this reserved block will actually be bigger than we need,
		//-- but we'll shrink it later.
		u8* pBuffer = m_MessageRecvBuffer.InsTail(NULL, header.GetDataSize());
		if (IS_NULL_PTR(pBuffer))
		{
			//-- Failed.
			//-- MessageRecvBuffer is too full.
			return Error::MessageRecvBufferFull;
		}

		//-- Decrypt packet payload data to MessageRecvBuffer.
		size_t nActualSize = SysString::KeyDecode(
			pBuffer, 
			header.GetDataSize(),
			(const s8*)m_PacketRecvBuffer.ConstBuffer() + header.GetHeaderSize(), 
			header.GetDataSize(),
			SysString::GenerateKey(0, 0));

		if (IS_ZERO(nActualSize))
		{
			//-- Failed.
			//-- Something went wrong with the Decode.
			//-- Revert InsTail.
			if (IS_NULL_PTR(m_MessageRecvBuffer.StripTail(header.GetDataSize())))
			{
				//-- Bad fail.
				//-- Couldn't revert InsTail!
				return Error::BadFail;
			}
			return Error::ProtocolMismatch;
		}

		//-- Success.
		//-- And because we InsTail'ed too much earlier, strip the remainder we didn't need.
		if (IS_NULL_PTR(m_MessageRecvBuffer.StripTail(header.GetDataSize() - nActualSize)))
		{
			//-- Bad fail.
			//-- Couldn't strip the unused remainder!
			return Error::BadFail;
		}
	}		
	else
	{
		//-- Just copy data to message buffer.
		if (IS_NULL_PTR(m_MessageRecvBuffer.InsTail(m_PacketRecvBuffer.ConstBuffer() + header.GetHeaderSize(), header.GetDataSize())))
		{
			//-- Failed.
			//-- MessageRecvBuffer is too full.
			return Error::MessageRecvBufferFull;
		}
	}

	//-- Success!
	
	//-- Increment message counter.
	m_nRecvMessages += header.GetMessageCount();
	//-- Strip header data off the PacketRecvBuffer.
	m_PacketRecvBuffer.StripHead(header.GetHeaderSize());
	//-- Strip data off the PacketRecvBuffer.
	m_PacketRecvBuffer.StripHead(header.GetDataSize());

	return Error::Ok;
}




CTCPConnection::Error::Enum CTCPConnection::Open(const s8* strHostname, u16 nHostPort)
{
	return Error::Ok;
}

CTCPConnection::Error::Enum	CTCPConnection::Close(void)
{
	return Error::Ok;
}


