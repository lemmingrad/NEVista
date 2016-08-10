#ifndef _SERVER_H_
#define _SERVER_H_
#pragma once

//----------------------------------------------------------//
// SERVER.H
//----------------------------------------------------------//
//-- Description
// Wrapper for 'server' functionality.
// Contains a list of CServerCli objects representing
// all the connected clients.
//----------------------------------------------------------//


#include "SysSmartPtr.h"
#include "ServerCli.h"
#include "TCPConnection.h"
#include "TCPNonBlockingAcceptor.h"
#include "Network.h"
#include "Message.h"


//----------------------------------------------------------//
// DEFINES
//----------------------------------------------------------//

//----------------------------------------------------------//
// ENUMS
//----------------------------------------------------------//

//----------------------------------------------------------//
// FORWARD REFERENCES
//----------------------------------------------------------//

//----------------------------------------------------------//
// STRUCTS
//----------------------------------------------------------//

//----------------------------------------------------------//
// CLASSES
//----------------------------------------------------------//


class CServer
{
	public:

		struct Error
		{
			enum Enum
			{
				WrongState			= 0x80002001,
				AcceptorFailed		= 0x80002002,

				//-- Badfail
				BadFail				= -1,

				//-- Success
				Ok					= 0,

				// In progress
				InProgress			= 1
			};
		};

		struct State
		{
			enum Enum
			{
				Idle = 0,
				Listening,	
				ShuttingDown
			};
		};

		typedef std::list<SysSmartPtr<CServerCli> >	TServerCliList;

		CServer();
		~CServer();

		Error::Enum							Initialise(void);
		Error::Enum							Shutdown(void);
		Error::Enum							Update(void);

		Error::Enum							Takedown(u32 nTime);

		Error::Enum							BroadcastMessage(SysSmartPtr<CMessage> pMessage, CServerCli* pException);

	private:

		TServerCliList						m_clientList;
		CTCPNonblockingAcceptor				m_acceptor;
		State::Enum							m_eState;
};


//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//

#endif //_SERVER_H_