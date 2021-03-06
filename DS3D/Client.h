#ifndef _CLIENT_H_
#define _CLIENT_H_
#pragma once

//----------------------------------------------------------//
// CLIENT.H
//----------------------------------------------------------//
//-- Description
// Client connection wrapper. Contains a TCP connection,
// message queues and utilities for handling incoming/outgoing
// messages.
//----------------------------------------------------------//


#include "SysSmartPtr.h"
#include "Network.h"
#include "TCPConnection.h"
#include "TCPNonBlockingConnector.h"
#include "Message.h"
#include <list>
#include <map>


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


class CClient
{
	public:

		struct Error
		{
			enum Enum
			{
				AlreadyRegistered	= 0x80000000,
				NotRegistered		= 0x80000001,
				WrongState			= 0x80000010,
				BadServerAddress	= 0x80000011,
				ConnectFailed		= 0x80000012,
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
				Connecting,
				Connected,
				LoggingIn,
				LoggedIn,
				Disconnecting
			};
		};

		typedef s32 (*TRecvMessageHandlerFunc)(SysSmartPtr<CMessage> pMessage, void* pData, bool bAlreadyProcessed);

		CClient();
		~CClient();

		Error::Enum		Initialise(void);
		Error::Enum		Shutdown(void);
		Error::Enum		Update(void);

		Error::Enum		Connect(s8* strServer);
		Error::Enum		Connect(const s8* strIP, const s8* strPort);
		Error::Enum		Disconnect(void);

		Error::Enum		AddRecvMessageHandler(CMessage::Type nType, TRecvMessageHandlerFunc handlerFunc, void* pData);
		Error::Enum		RemoveRecvMessageHandler(CMessage::Type nType, TRecvMessageHandlerFunc handlerFunc);

		Error::Enum		SendMessage(SysSmartPtr<CMessage> pMessage);

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

		typedef std::list<RecvMessageHandlerRecord> TRecvMessageHandlerRecordList;
		typedef std::map<CMessage::Type, TRecvMessageHandlerRecordList>	TRecvMessageHandlerArray;

		CTCPNonblockingConnector			m_connector;
		SysSmartPtr<CTCPConnection>			m_pConnection;

		TMessageList						m_recvMessageList;
		TMessageList						m_sendMessageList;

		TRecvMessageHandlerArray			m_RecvMessageHandlerArray;

		State::Enum							m_eState;

		Error::Enum							Reset(Error::Enum eError);

		Error::Enum							NotifyRecvMessageHandlers(void);

		static s32							ClientStateChangeHandler(SysSmartPtr<CMessage> pMessage, void* pData, bool bAlreadyHandled);
		static s32							ByeHandler(SysSmartPtr<CMessage> pMessage, void* pData, bool bAlreadyHandled);
};


//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//

#endif //_CLIENT_H_