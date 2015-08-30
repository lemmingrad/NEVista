#ifndef _TCPCONNECTOR_H_
#define _TCPCONNECTOR_H_
#pragma once

//----------------------------------------------------------//
// TCPCONNECTOR.H
//----------------------------------------------------------//
//-- Description			
// Establishes a TCP connection from a client end.
//----------------------------------------------------------//


#include "Types.h"
#include "SysSocket.h"
#include "Network.h"


//----------------------------------------------------------//
// DEFINES
//----------------------------------------------------------//

//----------------------------------------------------------//
// ENUMS
//----------------------------------------------------------//

//----------------------------------------------------------//
// FORWARD REFERENCES
//----------------------------------------------------------//


class CTCPConnection;


//----------------------------------------------------------//
// STRUCTS
//----------------------------------------------------------//

//----------------------------------------------------------//
// CLASSES
//----------------------------------------------------------//


class CTCPConnector
{
	public:

		struct Error
		{
			enum Enum
			{
				WrongState			= 0x80000000,
				GetInfoFail			= 0x80000001,
				NoMoreInfo			= 0x80000002,
				OpenFail			= 0x80000003,
				ConnectFail			= 0x80000004,
				SetSockOptFail		= 0x80000005,
				GetSockOptFail		= 0x80000006,
				SelectFail			= 0x80000007,

				BadFail				= -1,

				//-- Success
				Ok					= 0,
				
				//-- In progress
				InProgress			= 1
			};
		};

		struct Result
		{
			Result()
			: m_eError(Error::Ok)
			, m_pConnection(NULL)
			{		
			}

			Error::Enum			m_eError;
			CTCPConnection*		m_pConnection;
		};

		CTCPConnector();
		~CTCPConnector();

		Result					ConnectBlocking(const s8* strHostname, const s8* strPort);
		Error::Enum				ConnectNonblocking(const s8* strHostname, const s8* strPort);
		Result					Update(void);

	private:

		struct State
		{
			enum Enum
			{
				NotConnected = 0,
				GettingAddrInfo,
				NextAddr,
				Connecting,
				Connected,
			};
		};

		State::Enum				m_eState;

		SysSocket::AddrInfo*	m_pAddrResults;
		SysSocket::AddrInfo*	m_pCur;

		SysSocket::Socket		m_nSocket;


		void					CleanAddrResults(void);
		void					CleanSocket(void);
};


//----------------------------------------------------------//
// EXTERNALS
//----------------------------------------------------------//

//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//

#endif //_TCPCONNECTOR_H_