


#include "TCPConnection.h"
#include "Types.h"
#include "SysSocket.h"
#include "Packet.h"
#include "PacketSerializer.h"
#include "SimpleBuffer.h"
#include "Message.h"



CTCPConnection::CTCPConnection()
: m_eState(State::Unknown)
, m_nSocket(SysSocket::INVALID_SOCK)
{
	m_RecvBuffer.Clear();
	m_SendBuffer.Clear();
}

CTCPConnection::~CTCPConnection()
{
}

CTCPConnection::Error::Enum CTCPConnection::Initialise(void)
{
	m_nSocket = SysSocket::INVALID_SOCK;
	m_eState = State::Closed;
	m_RecvBuffer.Clear();
	m_SendBuffer.Clear();

	return Error::Ok;
}

CTCPConnection::Error::Enum CTCPConnection::Shutdown(void)
{
	return Close(Error::Ok);
}

CTCPConnection::Error::Enum CTCPConnection::Open(const s8* strHostname, const s8* strPort)
{
	assert(State::Closed == m_eState);
	if (State::Closed != m_eState)
	{
		return Error::WrongState;
	}

	if (SysSocket::INVALID_SOCK != m_nSocket)
	{
		//-- socket already set?
		return Error::WrongState;
	}

	SysSocket::AddrInfo hints;
	SysSocket::AddrInfo* pRes;

	SysMemory::Memclear(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	if (SYS_SOCKET_NO_ERROR != SysSocket::GetInfo(strHostname, strPort, &hints, &pRes))
	{
		return Close(Error::BadFail);
	}

/*	m_nSocket = SysSocket::OpenSocket(pRes->ai_family, pRes->ai_socktype, pRes->ai_protocol);
	if (SysSocket::INVALID_SOCK == m_nSocket)
	{
		return Close(Error::BadFail);
	}

	if (SYS_SOCKET_NO_ERROR != SysSocket::Connect(m_nSocket, pRes->ai_addr, pRes->ai_addrlen))
	{
		return Close(Error::BadFail);
	}

*/	m_eState = State::Open;
	m_RecvBuffer.Clear();
	m_SendBuffer.Clear();

	return Error::Ok;
}

CTCPConnection::Error::Enum	CTCPConnection::Close(CTCPConnection::Error::Enum eError)
{
	if (SysSocket::INVALID_SOCK != m_nSocket)
	{
		//-- Immediate shutdown of socket.
		SysSocket::CloseSocket(m_nSocket);
		m_nSocket = SysSocket::INVALID_SOCK;
	}

	m_eState = State::Closed;
	m_RecvBuffer.Clear();
	m_SendBuffer.Clear();

	return eError;
}

//-- We are trying to close the connection locally.
//-- 1. Put a 'bye' message in the send queue. No more messages should be added after that.
//-- 2. Wait for 'bye' to be packetized and sent. Send message queue should be empty afterwards.
//-- 3. Issue shutdown(write) locally. This tells the other end you aren't sending anymore, but are still receiving data.
//-- 4. Remote will receive the 'bye', send a 'byebye', and shutdown(write) their end, but continue to recv bytes.
//-- 5. Remote doing the shutdown(write) will stop it sending data back to local, and so eventually
//-- the local end will recv() 0 bytes.
//-- 6. When recv() 0 bytes, you can safely close() the local end of the socket.

//-- Remote end closed the connection.
//-- 1. Received a 'bye' packet. Send back a 'byebye'. 
//-- 2. Wait for 'byebye' to be sent.
//-- 3. Issue shutdown(write).
//-- 4. Wait for recv() to return 0 bytes.
//-- 5. close() local end of socket.


CTCPConnection::Error::Enum CTCPConnection::UpdateRecv(TMessageList& recvList)
{
	assert(State::Unknown != m_eState);
	if (State::Unknown == m_eState)
	{
		return Error::WrongState;
	}
	if (SysSocket::INVALID_SOCK == m_nSocket)
	{
//		return Error::WrongState;
	}
	
	if (IS_ZERO(m_RecvBuffer.UnusedSize()))
	{
		//-- If the buffer is ever completely full, we're in a bad place.
		//-- 1. No telling what a zero-length parameter to Recv() will do.
		//-- so best not to pass 0 length to Recv() at all.
		//-- 2. If the buffer is full it must contain an incomplete packet,
		//-- and with no space to recv more data to complete the packet, 
		//-- we'll get stuck forever. Return a BadFail.
		return Close(Error::BadFail);
	}

	//-- Update RecvBuffer with any new data from socket.
/*	s32 nBytes = SysSocket::Recv(m_nSocket, (s8*)m_RecvBuffer.Buffer() + m_RecvBuffer.UsedSize(), m_RecvBuffer.UnusedSize());
	if (SYS_SOCKET_WOULD_BLOCK == nBytes)
	{
		//-- No read activity, so nothing to do.
		return Error::Ok;
	}
	else if (SYS_SOCKET_RECV_ZERO == nBytes)
	{
		//-- Socket closed at remote end!
		return Close(Error::Closed);
	}
	else if (SYS_SOCKET_ERROR == nBytes)
	{
		//-- Error occurred (NOT EWOULDBLOCK)
		return Close(Error::BadFail);
	}
	
	if (IS_NULL_PTR(m_RecvBuffer.InsTail(NULL, nBytes)))
	{
		//-- Error. This should never happen, since InsTail(NULL, nBytes) should be fine if nBytes is valid.
		//-- (SysSocket::Recv must have returned a value >= m_RecvBuffer.UnusedSize())
		return Close(Error::BadFail);
	}
*/

	//-- If enough data is present, decrypt and copy received messages/contents as necessary.
	//-- Will loop until a packet error occurs or RecvBuffer is emptied.

	CPacket::Error::Enum ePacketError = CPacket::Error::Ok;
	while ( (CPacket::Error::Ok == ePacketError)
		&& (m_RecvBuffer.UsedSize() > 0) )
	{
		CPacket packet;
		CPacketSerializer packetDeserializer(CSerializer::Mode::Deserializing, m_RecvBuffer.Buffer(), m_RecvBuffer.UsedSize());

		//-- Test for valid packet header, decrypt, etc.
		ePacketError = packet.Serialize(packetDeserializer);
		if (CPacket::Error::Ok == ePacketError)
		{
			//-- Convert packet into messages, append to the received message list.
			ePacketError = packet.GetMessages(recvList);
		}

		switch (ePacketError)
		{
			case CPacket::Error::Ok:
			{
				//-- Success.
				if (IS_NULL_PTR(m_RecvBuffer.StripHead(packetDeserializer.GetOffset())))
				{
					//-- Error!
					//-- This should never happen since serializing and converting both succeeded, 
					//-- so there must have been sufficient data in RecvBuffer.
					//-- Since we've already added the data to the message list, we can't recover from any error here.
					//-- We must return BadFail.
					return Close(Error::BadFail);
				}
			}
			break;

			case CPacket::Error::DataBufferEmpty:
			case CPacket::Error::DataBufferFull:
			{
				//-- These are considered non-fatal errors, maybe due to a full or empty buffer.
				//-- In such cases, we'll assume the data is valid, and since nothing has been deleted yet we can try
				//-- to parse it again next time.
				//-- Do NOT strip head off RecvBuffer!
				return Error::Ok;
			}
			break;

			case CPacket::Error::Serializer:
			{
				CPacketSerializer::Error::Enum eSerError = packetDeserializer.GetError();
				switch (eSerError)
				{
					case CPacketSerializer::Error::FourCCMismatch:
					{
						//-- This is considered a protocol mismatch error.
						return Close(Error::ProtocolMismatch);
					}
					break;
					case CPacketSerializer::Error::Fail:
					case CPacketSerializer::Error::CopyFailed:
					case CPacketSerializer::Error::BadParameter:
					{
						//-- These are BadFail errors.
						return Close(Error::BadFail);
					}
					break;
					default:
					{
						//-- Other errors are considered non-fatal.
						//-- In such cases, we'll assume the data is valid, and since nothing has been deleted yet we can try
						//-- to parse it again next time.
						//-- Do NOT strip head off RecvBuffer!
						return Error::Ok;
					}
					break;
				}
			}
			break;

			//-- Bad Fails - Should force close.
			case CPacket::Error::UnknownVersion:
			case CPacket::Error::ProtocolMismatch:
			{
				return Close(Error::ProtocolMismatch);
			}
			break;
			case CPacket::Error::SanityFail:
			{
				return Close(Error::SanityFail);
			}
			break;
			default:
			{
				return Close(Error::BadFail);
			}
			break;
		}
	}

	return Error::Ok;
}


CTCPConnection::Error::Enum CTCPConnection::UpdateSend(TMessageList& sendList)
{
	assert(State::Unknown != m_eState);
	if (State::Unknown == m_eState)
	{
		return Error::WrongState;
	}
	if (SysSocket::INVALID_SOCK == m_nSocket)
	{
//		return Error::WrongState;
	}

	if ( (State::Closed == m_eState)
		|| (State::Closing_WaitingForRecvZero == m_eState) )
	{
		//-- Cannot send anything in these states.
		return Error::Ok;
	}

	CPacket::Error::Enum ePacketError = CPacket::Error::Ok;

	while ( (State::Closing_WaitingForEmptySend != m_eState)
		&& (CPacket::Error::Ok == ePacketError)
		&& IS_FALSE(sendList.empty())
		&& (m_SendBuffer.UnusedSize() > 0) )
	{
		//-- Add messages to packet.
		TMessageList::iterator it = sendList.begin();
		SysSmartPtr<CMessage> firstMessage = *it;
		bitfield bitFlags = 0;
		//SET_FLAG(bitFLags, CPacket::Flag::Encrypted);

		CPacket packet(bitFlags);
		CPacketSerializer packetSerializer(CSerializer::Mode::Serializing, m_SendBuffer.Buffer() + m_SendBuffer.UsedSize(), m_SendBuffer.UnusedSize());

		bool bByeDetected;
		ePacketError = packet.AddMessages(sendList, it, bByeDetected);
		if (CPacket::Error::Ok == ePacketError)
		{
			ePacketError = packet.Serialize(packetSerializer);
		}

		switch (ePacketError)
		{
			case CPacket::Error::Ok:
			{
				//-- Success.
				if (IS_PTR(m_SendBuffer.InsTail(NULL, packetSerializer.GetOffset())))
				{
					//-- Successfully copied some messages from sendList to the packet, and
					//-- then serialized the packet to the SendBuffer.
					//-- It's now safe to strip the consumed messges from sendList.
					if (IS_TRUE(bByeDetected))
					{
						//-- Packet contains a ByeBye messsage, which is the last message that 
						//-- should ever be sent before a graceful shutdown.
						//-- We can safely empty whatever is left in the sendList.
						sendList.clear();
						m_eState = State::Closing_WaitingForEmptySend;
					}
					else
					{
						//-- Not ByeBye in packet, so just erase the messages at the head of the
						//-- sendList that were consumed.
						sendList.erase(sendList.begin(), it);
					}
				}
				else
				{
					//-- Error!
					//-- This should never happen since serializing and converting both succeeded, 
					//-- so there must have been sufficient space in SendBuffer.
					return Close(Error::BadFail);
				}
			}
			break;

			case CPacket::Error::DataBufferEmpty:
			case CPacket::Error::DataBufferFull:
			{
				//-- These are considered non-fatal errors, maybe due to a full or empty buffer.
				//-- In such cases, we'll assume the data is valid, and since nothing has been deleted yet we can try
				//-- to parse it again next time.
				//-- Do NOT InsTail to SendBuffer!
				return Error::Ok;
			}
			break;

			case CPacket::Error::Serializer:
			{
				CPacketSerializer::Error::Enum eSerError = packetSerializer.GetError();
				switch (eSerError)
				{
					case CPacketSerializer::Error::FourCCMismatch:
					{
						//-- This is considered a protocol mismatch error.
						return Close(Error::ProtocolMismatch);
					}
					break;
					case CPacketSerializer::Error::Fail:
					case CPacketSerializer::Error::CopyFailed:
					case CPacketSerializer::Error::BadParameter:
					{
						//-- These are BadFail errors.
						return Close(Error::BadFail);
					}
					break;
					default:
					{
						//-- Other errors are considered non-fatal.
						//-- In such cases, we'll assume the data is valid, and since nothing has been deleted yet we can try
						//-- to parse it again next time.
						//-- Do NOT strip head off RecvBuffer!
						return Error::Ok;
					}
					break;
				}
			}
			break;

			//-- Bad Fails - Should force close.
			case CPacket::Error::UnknownVersion:
			case CPacket::Error::ProtocolMismatch:
			{
				return Close(Error::ProtocolMismatch);
			}
			break;
			case CPacket::Error::SanityFail:
			{
				return Close(Error::SanityFail);
			}
			break;
			default:
			{
				return Close(Error::BadFail);
			}
			break;
		}
	}
	
	//-- Send to socket
	if (m_SendBuffer.UsedSize() > 0)
	{
//		s32 nBytes = SysSocket::Send(m_nSocket, (const s8*)m_SendBuffer.ConstBuffer(), m_SendBuffer.UsedSize());
		m_RecvBuffer.InsTail(m_SendBuffer.ConstBuffer(), m_SendBuffer.UsedSize());
		s32 nBytes = m_SendBuffer.UsedSize();
		if (nBytes > 0)
		{
			if (IS_NULL_PTR(m_SendBuffer.StripHead(nBytes)))
			{
				return Close(Error::BadFail);
			}
		}
	}

	if ( IS_ZERO(m_SendBuffer.UsedSize()) 
		&& (State::Closing_WaitingForEmptySend == m_eState) )
	{
//		SysSocket::Shutdown(m_nSocket, SysSocket::Shutdown::Write);
		m_eState = State::Closing_WaitingForRecvZero;
	}

	return Error::Ok;
}


