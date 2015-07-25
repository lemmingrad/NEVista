#ifndef _TCPCONNECTION_H_
#define _TCPCONNECTION_H_
#pragma once


#include "Types.h"
#include "SimpleBuffer.h"
#include <list>


#define MAX_PACKET_SIZE				(1500)
#define TCP_RECV_BUFFER_SIZE		(MAX_PACKET_SIZE * 4)
#define TCP_SEND_BUFFER_SIZE		(MAX_PACKET_SIZE * 4)



class CTCPConnection
{
	public:

//		typedef std::list<CMessage*> TMessageList;

		struct Error
		{
			enum Enum
			{
				//-- Not so bad fails
				MessageRecvBufferFull		= 0x80000000,
				MessageRecvBufferEmpty		= 0x80000001,
				PacketRecvBufferFull		= 0x80000002,
				PacketRecvBufferEmpty		= 0x80000003,
				CopyFailed					= 0x80000004,

				//-- Bad fails
				UnknownVersion				= 0x80001000,
				ProtocolMismatch			= 0x80001001,
				SanityFail					= 0x80001002,
				
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
				Opening,
				Open,
				KeyExchange,
				Encrypted,
				Active,
				Closing
			};
		};


		CTCPConnection();
		~CTCPConnection();

		Error::Enum			Initialise(void);
		Error::Enum			Shutdown(void);
		Error::Enum			Update(void);

		Error::Enum			Open(const s8* strHostname, const s8* strHostPort);
		Error::Enum			Close(void);

	private:

		Error::Enum			UpdateRecv(void);


		CSimpleBuffer<TCP_RECV_BUFFER_SIZE>			m_RecvBuffer;
		CSimpleBuffer<TCP_SEND_BUFFER_SIZE>			m_SendBuffer;
	
//		TMessageList								m_RecvMessages;
//		TMessageList								m_SendMessages;

		State::Enum									m_eState;
};









#endif //_TCPCONNECTION_H_