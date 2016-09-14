//----------------------------------------------------------//
// TCPCONNECTION.CPP
//----------------------------------------------------------//
//-- Description			
// Wrapper around a TCP socket with send and receive 
// buffering and to/from Packet conversion.
//----------------------------------------------------------//


#include "TCPConnection.h"
#include "Types.h"
#include "SysSocket.h"
#include "Packet.h"
#include "PacketSerializer.h"
#include "SimpleBuffer.h"
#include "Message.h"
#include "Messages/MsgBye.h"
#include "Messages/MsgMotd.h"
#include "Messages/MsgClientKeyExchange.h"
#include "Messages/MsgServerKeyExchange.h"
#include "SysDebugLog.h"


//----------------------------------------------------------//
// DEFINES
//----------------------------------------------------------//

//----------------------------------------------------------//
// GLOBALS
//----------------------------------------------------------//

//----------------------------------------------------------//
// CTCPConnection::CTCPConnection
//----------------------------------------------------------//
//-- Description			
//----------------------------------------------------------//
CTCPConnection::CTCPConnection(SysSocket::Socket nSocket)
: m_eState(State::Closed)
, m_nSocket(SysSocket::INVALID_SOCK)
, m_Key(SysString::INVALID_KEY)
, m_nClientRnd(0)
, m_nServerRnd(0)
{
	//-- Just touch these, otherwise they never get added to the CMessageFactory.
	CMsgMotd dummy;

	if (SysSocket::INVALID_SOCK != nSocket)
	{
		m_eState = State::Open;
		m_nSocket = nSocket;
		m_nClientRnd = 0;	// TODO - random u8
		m_nServerRnd = 0;	// TODO - random u8
	}

	m_RecvBuffer.Clear();
	m_SendBuffer.Clear();
}


//----------------------------------------------------------//
// CTCPConnection::~CTCPConnection
//----------------------------------------------------------//
//-- Description			
//----------------------------------------------------------//
CTCPConnection::~CTCPConnection()
{
	Close(Error::Ok);
}


//----------------------------------------------------------//
// CTCPConnection::Close
//----------------------------------------------------------//
//-- Description			
//----------------------------------------------------------//
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


//----------------------------------------------------------//
// CTCPConnection::UpdateRecv
//----------------------------------------------------------//
//-- Description			
// Calls recv() and add any pending socket data to a buffer.
// Attempts to form a complete packet from the buffer.
// Parse the packet and convert into a list of messages to
// be appended to recvList.
// Note that if this CTCPConnection is from the result of
// a CTCPBlockingConnector or CTCPBlockingAcceptor, it will
// block on the recv().
// Internally handles encryption handshaking and safe
// shutdown when necessary.
//----------------------------------------------------------//
CTCPConnection::Error::Enum CTCPConnection::UpdateRecv(TMessageList& recvList, TMessageList& sendList)
{
	if (SysSocket::INVALID_SOCK == m_nSocket)
	{
		return Error::WrongState;
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
	s32 nBytes = SysSocket::Recv(m_nSocket, (s8*)m_RecvBuffer.Buffer() + m_RecvBuffer.UsedSize(), m_RecvBuffer.UnusedSize());

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
	
	SysDebugPrintf("Received %d bytes.\n", nBytes);

	if (IS_NULL_PTR(m_RecvBuffer.InsTail(NULL, nBytes)))
	{
		//-- Error. This should never happen, since InsTail(NULL, nBytes) should be fine if nBytes is valid.
		//-- (SysSocket::Recv must have returned a value >= m_RecvBuffer.UnusedSize())
		return Close(Error::BadFail);
	}

	//-- If enough data is present, decrypt and copy received messages/contents as necessary.
	//-- Will loop until a packet error occurs or RecvBuffer is emptied.

	CPacket::Error::Enum ePacketError = CPacket::Error::Ok;
	while ( (CPacket::Error::Ok == ePacketError)
		&& (m_RecvBuffer.UsedSize() > 0) )
	{
		CPacket packet(m_Key);
		CPacketSerializer packetDeserializer(ISerializer::Mode::Deserializing, m_RecvBuffer.Buffer(), m_RecvBuffer.UsedSize());

		TMessageList packetMessagesList;

		//-- Test for valid packet header, decrypt, etc.
		ePacketError = packet.Serialize(packetDeserializer);
		if (CPacket::Error::Ok == ePacketError)
		{
			//-- Get packet data as a list of messages
			ePacketError = packet.GetMessages(packetMessagesList);
		}

		SysDebugPrintf("Packet Received\n");
		SysDebugPrintf("  version: %d, data size: %d\n", packet.Validate(), packet.GetDataSize());
		SysDebugPrintf("  flags: %s %s\n", 
			packet.IsCompressed() ? "compressed" : "",
			packet.IsEncrypted() ? "encrypted" : "");
		SysDebugPrintf("  messages: %d expected, %d parsed\n", packet.GetMessageCount(), packetMessagesList.size());

		switch (ePacketError)
		{
			case CPacket::Error::Ok:
			{
				//-- Success.
				if (IS_PTR(m_RecvBuffer.StripHead(packetDeserializer.GetOffset())))
				{
					//-- Success, so safe to append the packetMessages list to the end of the
					//-- recvList.
					for (TMessageList::const_iterator cit = packetMessagesList.begin(); cit != packetMessagesList.end(); ++cit)
					{
						SysSmartPtr<CMessage> pMessage = *cit;

						SysDebugPrintf("  Message: \"%s\"\n", CMessageFactory::GetTypeString(pMessage->GetType()));

						switch (pMessage->GetType())
						{
							case CMsgBye::kType:
							{
								//-- Incoming packet contained a Bye message.
								//-- Send a Bye-ack back.
								SysSmartPtr<CMessage> pResponse(new CMsgBye(CMsgBye::Reason::ByeAcknowledged));
								if (IS_PTR(pResponse.get()))
								{
									sendList.push_back(pResponse);
								}
							}
							break;
							case CMsgMotd::kType:
							{			
								//-- Incoming packet contained a Motd message. 
								//-- This means we must be the client.
								//-- Correct response is to send a ClientKeyExchange back to server.
								SysSmartPtr<CMessage> pResponse(new CMsgClientKeyExchange());
								CMsgClientKeyExchange* pKeyEx = (CMsgClientKeyExchange*)pResponse.get();
								if (IS_PTR(pKeyEx))
								{
									pKeyEx->SetKey(m_nClientRnd);
									sendList.push_back(pResponse);
								}
							}
							break;
							case CMsgClientKeyExchange::kType:
							{			
								//-- Incoming packet contained a ClientKeyExchange message. 
								//-- This means we must be the server.
								//-- Correct response is to send a ServerKeyExchange back to client.
								//-- And we must generate an encryption key (which should be identical on client and server).
								CMsgClientKeyExchange* pIn = (CMsgClientKeyExchange*)pMessage.get();
								if (IS_PTR(pIn))
								{
									m_nClientRnd = pIn->GetKey();
									m_Key = SysString::GenerateKey(m_nServerRnd, m_nClientRnd);
							
									SysSmartPtr<CMessage> pResponse(new CMsgServerKeyExchange());
									CMsgServerKeyExchange* pKeyEx = (CMsgServerKeyExchange*)pResponse.get();
									if (IS_PTR(pKeyEx))
									{
										pKeyEx->SetKey(m_nServerRnd);
										sendList.push_back(pResponse);
									}
								}
							}
							break;
							case CMsgServerKeyExchange::kType:
							{			
								//-- Incoming packet contained a ServerKeyExchange message. 
								//-- This means we must be the client.
								//-- Generate an encryption key (which should be identical on client and server).
								CMsgServerKeyExchange* pIn = (CMsgServerKeyExchange*)pMessage.get();
								if (IS_PTR(pIn))
								{
									m_nServerRnd = pIn->GetKey();
									m_Key = SysString::GenerateKey(m_nServerRnd, m_nClientRnd);
								}
							}
							break;
							default:
							break;
						}
					}

					recvList.splice(recvList.end(), packetMessagesList);
				}
				else
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
			case CPacket::Error::EncryptionFailed:
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


//----------------------------------------------------------//
// CTCPConnection::UpdateSend
//----------------------------------------------------------//
//-- Description			
// Attempt to form a packet using the head messages of
// sendList.
// Serialize the packet data to a buffer.
// Calls send() to transfer anything in the buffer to 
// the network.
// Note that if this CTCPConnection is from the result of
// a CTCPBlockingConnector or CTCPBlockingAcceptor, it will
// block on the send().
// Internally handles encryption handshaking and safe
// shutdown when necessary.
//----------------------------------------------------------//
CTCPConnection::Error::Enum CTCPConnection::UpdateSend(TMessageList& sendList)
{
	if (SysSocket::INVALID_SOCK == m_nSocket)
	{
		return Error::WrongState;
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
		CPacket packet(m_Key);
		CPacketSerializer packetSerializer(ISerializer::Mode::Serializing, m_SendBuffer.Buffer() + m_SendBuffer.UsedSize(), m_SendBuffer.UnusedSize());

		TMessageList::iterator after;

		ePacketError = packet.AddMessages(sendList, after);
		if (CPacket::Error::Ok == ePacketError)
		{
			ePacketError = packet.Serialize(packetSerializer);
		}

		SysDebugPrintf("Packet Sending\n");
		SysDebugPrintf("  version: %d, data size: %d\n", packet.Validate(), packet.GetDataSize());
		SysDebugPrintf("  flags: %s %s\n", 
			packet.IsCompressed() ? "compressed" : "",
			packet.IsEncrypted() ? "encrypted" : "");

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
					bool bHasBye = false;
					
					for (TMessageList::const_iterator cit = sendList.begin(); cit != after; ++cit)
					{
						SysSmartPtr<CMessage> pMessage = *cit;

						SysDebugPrintf("  Message: \"%s\"\n", CMessageFactory::GetTypeString(pMessage->GetType()));

						if (CMsgBye::kType == pMessage->GetType())
						{
							bHasBye = true;
							break;
						}
					}

					if (IS_TRUE(bHasBye))
					{
						//-- Packet contains a Bye messsage, which is the last message that 
						//-- should ever be sent before a graceful shutdown.
						//-- We can safely empty whatever is left in the sendList.
						sendList.clear();
						m_eState = State::Closing_WaitingForEmptySend;
					}
					else
					{
						//-- No Bye in packet, so just erase the messages at the head of the
						//-- sendList that were consumed by packet.AddMessages().
						sendList.erase(sendList.begin(), after);
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
		s32 nBytes = SysSocket::Send(m_nSocket, (const s8*)m_SendBuffer.ConstBuffer(), m_SendBuffer.UsedSize());
		if (nBytes > 0)
		{
			if (IS_NULL_PTR(m_SendBuffer.StripHead(nBytes)))
			{
				return Close(Error::BadFail);
			}

			SysDebugPrintf("Sent %d bytes.\n", nBytes);
		}
	}

	if ( IS_ZERO(m_SendBuffer.UsedSize()) 
		&& (State::Closing_WaitingForEmptySend == m_eState) )
	{
		SysSocket::Shutdown(m_nSocket, SysSocket::Shutdown::Write);
		m_eState = State::Closing_WaitingForRecvZero;
	}

	return Error::Ok;
}


//----------------------------------------------------------//
// EXTERNALS
//----------------------------------------------------------//

//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//
