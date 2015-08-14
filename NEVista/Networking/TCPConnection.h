#ifndef _TCPCONNECTION_H_
#define _TCPCONNECTION_H_
#pragma once


#include "Types.h"
#include "SysSmartPtr.h"
#include "SysSocket.h"
#include "SimpleBuffer.h"
#include "Network.h"


#define MAX_PACKET_SIZE				(1500)
#define TCP_RECV_BUFFER_SIZE		(MAX_PACKET_SIZE * 4)
#define TCP_SEND_BUFFER_SIZE		(MAX_PACKET_SIZE * 4)


class CMessage;

class CTCPConnection
{
	public:

		struct Error
		{
			enum Enum
			{
				//-- Not so bad fails
				RecvBufferFull				= 0x80000000,
				RecvBufferEmpty				= 0x80000001,
				SendBufferFull				= 0x80000002,
				SendBufferEmpty				= 0x80000003,

				//-- Bad fails
				UnknownVersion				= 0x80001000,
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
				Unknown = 0,
				Closed,
				Open,
				Closing_WaitingForEmptySend,
				Closing_WaitingForRecvZero
			};
		};


		CTCPConnection();
		~CTCPConnection();

		Error::Enum			Initialise(void);
		Error::Enum			Shutdown(void);
		Error::Enum			UpdateRecv(TMessageList& pushList);
		Error::Enum			UpdateSend(TMessageList& pullList);

		Error::Enum			Open(const s8* strHostname, const s8* strHostPort);
		Error::Enum			Close(Error::Enum eError);

	private:

		SysSocket::Socket							m_nSocket;

		CSimpleBuffer<TCP_RECV_BUFFER_SIZE>			m_RecvBuffer;
		CSimpleBuffer<TCP_SEND_BUFFER_SIZE>			m_SendBuffer;
	
		State::Enum									m_eState;
};









#endif //_TCPCONNECTION_H_