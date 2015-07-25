


#include "TCPConnection.h"
#include "Types.h"
#include "SysSocket.h"
#include "Packet.h"
#include "PacketSerializer.h"
#include "SimpleBuffer.h"



CTCPConnection::CTCPConnection()
: m_eState(State::Unknown)
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
	//-- Check for socket recv

	//-- Check for TCP received data.
	//-- If enough data is present, decrypt and copy received messages/contents as necessary.

	while (m_RecvBuffer.UsedSize() >= CPacket::GetMinimumReadSize())
	{
		CPacket packet;
		CPacketSerializer packetDeserializer(CSerializer::Mode::Deserializing, m_RecvBuffer.Buffer(), m_RecvBuffer.UsedSize());

		CPacket::Error::Enum eError = packet.Serialize(packetDeserializer);
		switch (eError)
		{
			case CPacket::Error::Ok:
			{
				//-- Success.
				//-- Add packet to packet list
				//-- Strip head off PacketRecvBuffer
				m_RecvBuffer.StripHead(packetDeserializer.GetOffset());




/*
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
*/
			}
			break;

			//-- Other fail
			case CPacket::Error::DataBufferFull:
			{
				//-- Non-fatal error, maybe a full or empty buffer.
				//-- Do NOT strip head off PacketRecvBuffer, so we can try again next time.
				return Error::Ok;
			}
			break;

			//-- Bad fail
			case CPacket::Error::BadFail:
			default:
			{
				//-- Fatal error. Should force a connection reset.
				//-- Do NOT strip head off PacketRecvBuffer.
				return Error::BadFail;
			}
			break;
		}
	}

	return Error::Ok;
}

/*
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
*/



CTCPConnection::Error::Enum CTCPConnection::Open(const s8* strHostname, const s8* strPort)
{
	SysSocket::AddrInfo hints;
	SysSocket::AddrInfo* pRes;

	SysMemory::Memclear(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	if (SYS_SOCKET_NO_ERROR != SysSocket::GetInfo(strHostname, strPort, &hints, &pRes))
	{
		return Error::BadFail;
	}

	SysSocket::Socket aSocket = SysSocket::OpenSocket(pRes->ai_family, pRes->ai_socktype, pRes->ai_protocol);
	if (SysSocket::INVALID_SOCK == aSocket)
	{
		return Error::BadFail;
	}

	if (SYS_SOCKET_NO_ERROR != SysSocket::Connect(aSocket, pRes->ai_addr, pRes->ai_addrlen))
	{
		return Error::BadFail;
	}

	return Error::Ok;
}

CTCPConnection::Error::Enum	CTCPConnection::Close(void)
{
	return Error::Ok;
}


