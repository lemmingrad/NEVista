#ifndef _TCPCONNECTION_H_
#define _TCPCONNECTION_H_
#pragma once

//----------------------------------------------------------//
// TCPCONNECTION.H
//----------------------------------------------------------//
//-- Description			
// Wrapper around a TCP socket with send and receive 
// buffering and to/from Packet conversion.
//----------------------------------------------------------//


#include "Types.h"
#include "SysString.h"
#include "SysSmartPtr.h"
#include "SysSocket.h"
#include "SimpleBuffer.h"
#include "Network.h"


//----------------------------------------------------------//
// DEFINES
//----------------------------------------------------------//


#define MAX_PACKET_SIZE				(1500)
#define TCP_RECV_BUFFER_SIZE		(MAX_PACKET_SIZE * 4)
#define TCP_SEND_BUFFER_SIZE		(MAX_PACKET_SIZE * 4)


//----------------------------------------------------------//
// ENUMS
//----------------------------------------------------------//

//----------------------------------------------------------//
// FORWARD REFERENCES
//----------------------------------------------------------//


class CMessage;


//----------------------------------------------------------//
// STRUCTS
//----------------------------------------------------------//

//----------------------------------------------------------//
// CLASSES
//----------------------------------------------------------//


class CTCPConnection
{
	public:

		struct Error
		{
			enum Enum
			{
				//-- Bad fails
				ProtocolMismatch			= 0x80001001,
				SanityFail					= 0x80001002,
				CopyFailed					= 0x80001003,
				
				//-- State
				WrongState					= 0x80002000,
				Closed						= 0x80002001,
				
				BadFail = -1,

				//-- Success
				Ok = 0
			};
		};

		struct State
		{
			enum Enum
			{
				Closed = 0,
				Open,
				Closing_WaitingForEmptySend,
				Closing_WaitingForRecvZero
			};
		};


		CTCPConnection(SysSocket::Socket nSocket);
		~CTCPConnection();

		Error::Enum			UpdateRecv(TMessageList& recvList, TMessageList& sendList);
		Error::Enum			UpdateSend(TMessageList& sendList);

		SysSocket::Socket	GetSocket(void);

		Error::Enum			Close(Error::Enum eError);

	private:

		SysSocket::Socket							m_nSocket;

		CSimpleBuffer<TCP_RECV_BUFFER_SIZE>			m_RecvBuffer;
		CSimpleBuffer<TCP_SEND_BUFFER_SIZE>			m_SendBuffer;
	
		State::Enum									m_eState;

		SysString::Key								m_Key;
		u8											m_nClientRnd;
		u8											m_nServerRnd;
};


//----------------------------------------------------------//
// EXTERNALS
//----------------------------------------------------------//

//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//

#endif //_TCPCONNECTION_H_