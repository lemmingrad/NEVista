#ifndef _TCPCONNECTION_H_
#define _TCPCONNECTION_H_
#pragma once


#include "Types.h"
#include "SimpleBuffer.h"

#define MAX_PACKET_SIZE				(1500)
#define MAX_MESSAGE_SIZE			(1200)
#define TCP_RECV_BUFFER_SIZE		(MAX_PACKET_SIZE * 4)
#define TCP_SEND_BUFFER_SIZE		(MAX_PACKET_SIZE * 4)
#define MSG_RECV_BUFFER_SIZE		(MAX_MESSAGE_SIZE * 4)
#define MSG_SEND_BUFFER_SIZE		(MAX_MESSAGE_SIZE * 4)



class CTCPConnection
{
	public:

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
				Closing,
			};
		};


		CTCPConnection();
		~CTCPConnection();

		Error::Enum			Initialise(void);
		Error::Enum			Shutdown(void);
		Error::Enum			Update(void);

		Error::Enum			Open(const s8* strHostname, u16 nHostPort);
		Error::Enum			Close(void);

	private:

		Error::Enum			UpdateRecv(void);
		Error::Enum			UpdateRecvV1(CPacketHeader& header);



		CSimpleBuffer<TCP_RECV_BUFFER_SIZE>			m_PacketRecvBuffer;
		CSimpleBuffer<TCP_SEND_BUFFER_SIZE>			m_PacketSendBuffer;

		CSimpleBuffer<MSG_RECV_BUFFER_SIZE>			m_MessageRecvBuffer;
		CSimpleBuffer<MSG_SEND_BUFFER_SIZE>			m_MessageSendBuffer;
		u32											m_nRecvMessages;
		u32											m_nSendMessages;

		State::Enum									m_eState;


};









#endif //_TCPCONNECTION_H_