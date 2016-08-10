#ifndef _TCPACCEPTOR_H_
#define _TCPACCEPTOR_H_
#pragma once

//----------------------------------------------------------//
// TCPACCEPTOR.H
//----------------------------------------------------------//
//-- Description			
// Establishes a TCP listener on a sevrer end, handles new
// incoming connection.
//----------------------------------------------------------//


#include "Types.h"
#include "SysSocket.h"
#include "SysMemory.h"
#include "Network.h"


//----------------------------------------------------------//
// DEFINES
//----------------------------------------------------------//


#define TCP_ACCEPTOR_BACKLOG	10


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


class CTCPAcceptor
{
	public:

		struct Error
		{
			enum Enum
			{
				WrongState				= 0x80000000,
				GetInfoFail				= 0x80000001,
				NoMoreInfo				= 0x80000002,
				OpenFail				= 0x80000003,
				SetSockOptFail			= 0x80000004,
				SetReusableFail			= 0x80000005,
				BindFail				= 0x80000006,
				ListenFail				= 0x80000007,
				SelectFail				= 0x80000008,

				BadFail					= -1,

				//-- Success
				Ok						= 0,		

				//-- In progress
				InProgress				= 1
			};
		};

		struct Result
		{
			Result()
			: m_eError(Error::Ok)
			, m_pConnection(NULL)
			, m_address(NULL)
			{
			}

			Error::Enum						m_eError;
			SysSmartPtr<CTCPConnection>		m_pConnection;
			SysSocket::Address				m_address;
		};


		CTCPAcceptor();
		~CTCPAcceptor();

		Error::Enum							StopListening(void);

	protected:

		struct State
		{
			enum Enum
			{
				NotListening = 0,
				GettingAddrInfo,
				NextAddr,
				Listening
			};
		};

		State::Enum							m_eState;

		SysSocket::AddrInfo*				m_pAddrResults;
		SysSocket::AddrInfo*				m_pCur;

		SysSocket::Socket					m_nSocket;

		void								CleanAddrResults(void);
		void								CleanSocket(void);
};


//----------------------------------------------------------//
// EXTERNALS
//----------------------------------------------------------//

//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//

#endif //_TCPACCEPTOR_H_