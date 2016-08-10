#ifndef _SERVERCLI_H_
#define _SERVERCLI_H_
#pragma once

//----------------------------------------------------------//
// SERVERCLI.H
//----------------------------------------------------------//
//-- Description
// Class that spawns for every incoming client connection,
// Contains a TCP connection, message queues and utilities 
// for handling incoming/outgoing messages.
// Equivalent functionality to CClient class used by
// client code.
//----------------------------------------------------------//


#include "SysSmartPtr.h"
#include "SysSocket.h"
#include "FixedString.h"
#include "Network.h"
#include "TCPConnection.h"
#include "Message.h"
#include <list>
#include <map>


//----------------------------------------------------------//
// DEFINES
//----------------------------------------------------------//


#define SERVERCLI_LOGIN_NAME_MAX_SIZE		(128)			


//----------------------------------------------------------//
// ENUMS
//----------------------------------------------------------//

//----------------------------------------------------------//
// FORWARD REFERENCES
//----------------------------------------------------------//


class CServer;


//----------------------------------------------------------//
// STRUCTS
//----------------------------------------------------------//

//----------------------------------------------------------//
// CLASSES
//----------------------------------------------------------//


class CServerCli
{
	public:

		struct Error
		{
			enum Enum
			{
				AlreadyRegistered	= 0x80000000,
				NotRegistered		= 0x80000001,
				WrongState			= 0x80000010,		
				ConnectionClosed	= 0x80000020,
				ConnectionError		= 0x80000021,

				//-- Message related
				NullMessage			= 0x80001000,
				LoginFailed			= 0x80001001,

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
				Disconnected = 0,
				Connected,
				LoggedIn,
				Disconnecting
			};
		};

		typedef s32 (*TRecvMessageHandlerFunc)(SysSmartPtr<CMessage> pMessage, void* pData, bool bAlreadyProcessed);

		CServerCli(CServer& server);
		~CServerCli();

		Error::Enum							Initialise(SysSmartPtr<CTCPConnection> pConnection, SysSocket::Address& address);
		Error::Enum							Shutdown(void);
		Error::Enum							Update(void);

		Error::Enum							Disconnect(void);

		Error::Enum							AddRecvMessageHandler(CMessage::Type nType, TRecvMessageHandlerFunc handlerFunc, void* pData);
		Error::Enum							RemoveRecvMessageHandler(CMessage::Type nType, TRecvMessageHandlerFunc handlerFunc);

		Error::Enum							SendMessage(SysSmartPtr<CMessage> pMessage);

	private:

		struct RecvMessageHandlerRecord
		{
			RecvMessageHandlerRecord(TRecvMessageHandlerFunc pCB, void* pData)
			: m_pCB(pCB)
			, m_pData(pData)
			{
			}

			TRecvMessageHandlerFunc			m_pCB;
			void*							m_pData;
		};

		typedef std::list<RecvMessageHandlerRecord>							TRecvMessageHandlerRecordList;
		typedef std::map<CMessage::Type, TRecvMessageHandlerRecordList>		TRecvMessageHandlerArray;

		class Messaging
		{
			public:
				Messaging(); 
				~Messaging();

				Initialise();
				Shutdown();
				Update();
				Reset();
			private:
				TMessageList					m_recvMessageList;
				TMessageList					m_sendMessageList;
				TRecvMessageHandlerArray		m_receivedMessageHandlers;
		};

		struct ConnectionVars
		{
			SysSmartPtr<CTCPConnection>		m_pConnection;
			SysSocket::Address				m_connectionAddress;
		};

		struct LoginVars
		{
			FixedString<SERVERCLI_LOGIN_NAME_MAX_SIZE> m_strLoginEmail;
		};

		ConnectionVars						m_connectionVars;
		MessagingVars						m_messagingVars;
		LoginVars							m_loginVars;
		CServer&							m_server;
		State::Enum							m_eState;

		Error::Enum							Reset(Error::Enum eError);
		Error::Enum							NotifyRecvMessageHandlers(void);

		static s32							ClientStateChangeHandler(SysSmartPtr<CMessage> pMessage, void* pData, bool bAlreadyHandled);
		static s32							ByeHandler(SysSmartPtr<CMessage> pMessage, void* pData, bool bAlreadyHandled);
};


//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//

#endif //_SERVERCLI_H_