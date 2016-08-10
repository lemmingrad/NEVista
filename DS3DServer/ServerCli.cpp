//----------------------------------------------------------//
// SERVERCLI.CPP
//----------------------------------------------------------//
//-- Description
// Class that spawns for every incoming client connection,
// Contains a TCP connection, message queues and utilities 
// for handling incoming/outgoing messages.
// Equivalent functionality to CClient class used by
// client code.
//----------------------------------------------------------//


#include "ServerCli.h"
#include "SystemIncludes.h"
#include "TCPConnection.h"
#include "TCPNonBlockingConnector.h"
#include "Network.h"
#include "Message.h"
#include "Messages/MsgBye.h"
#include "Messages/MsgMotd.h"
#include "Messages/MsgLoginRequest.h"
#include "Messages/MsgLoginResponse.h"

#include "SQLiteDatabase.h"
#include "EventLogSQLTable.h"
#include "UsersSQLTable.h"


//----------------------------------------------------------//
// DEFINES
//----------------------------------------------------------//

//----------------------------------------------------------//
// GLOBALS
//----------------------------------------------------------//

extern CSQLiteDatabase gDatabase;



CServerCli::CServerCli(CServer& server)
: m_server(server)
, m_pConnection()
, m_connectionAddress(NULL)
, m_eState(State::Disconnected)
{
	//-- Just touch these, otherwise they never get added to the CMessageFactory.

	m_messagingVars.m_receivedMessageHandlers.clear();

	Reset(Error::Ok);
}


CServerCli::~CServerCli()
{
	Shutdown();
}


CServerCli::Error::Enum CServerCli::Initialise(SysSmartPtr<CTCPConnection> pConnection, SysSocket::Address& address)
{
	//-- Reset everything.
	Reset(Error::Ok);

	//-- Set up received message handlers.
	m_messaging.Reset();
//	m_messaging.m_receivedMessageHandlers.clear();
	//-- Register a handler to process incoming messages related to logging in.
	m_messaging.AddReceivedMessageHandler(CMsgLoginRequest::kType, &CServerCli::ClientStateChangeHandler, this);
	//-- Register a handler to process incoming CMsgBye messages
	m_messaging.AddReceivedMessageHandler(CMsgBye::kType, &CServerCli::ByeHandler, this);
	
	m_eState = State::Connected;

	m_connection.Initialise(pConnection, address);
//	m_connection.m_pConnection = pConnection;
//	m_connection.m_connectionAddress = address;

	//-- Update SQL database.
	const s8* strIP = m_connection.GetIPAddress();
//	FixedString<SYSSOCKET_IPADDRESS_MAX_SIZE> strIP;
//	m_connectionVars.m_connectionAddress.GetIPAddress(strIP.Buffer(), strIP.Size());

	FixedString<EVENTLOG_DETAILS_MAX_SIZE> strDetailsText;
	strDetailsText = "Connection attempt made from ";
	strDetailsText += strIP;

	CEventLogSQLTable eventLog(gDatabase);
	CEventLogEvent inEvent("Connection", strDetailsText.ConstBuffer());
	eventLog.AddEvent(inEvent);

	//-- Send MOTD message to client.
	SysSmartPtr<CMessage> pMessage(new CMsgMotd());
	CMsgMotd* pMotd = (CMsgMotd*)pMessage.ptr();
	if (IS_PTR(pMotd))
	{
		pMotd->SetText("Hello, welcome to NEVista.");
		return SendMessage(pMessage);
	}

	return Reset(Error::BadFail);
}


CServerCli::Error::Enum CServerCli::Shutdown(void)
{
	//-- Not a nice disconnect!
	m_messaging.Reset();
	//m_receivedMessageHandlers.clear();

	return Reset(Error::Ok);
}


CServerCli::Error::Enum CServerCli::Reset(CServerCli::Error::Enum eError)
{
	m_eState = State::Disconnected;

	MessagingVars& messagingVars = m_messagingVars;

	messagingVars.m_recvMessageList.clear();
	messagingVars.m_sendMessageList.clear();

	ConnectionVars& connectionVars = m_connectionVars;

	if (IS_PTR(connectionVars.m_pConnection.ptr()))
	{
		connectionVars.m_connectionAddress = SysSocket::Address();

		connectionVars.m_pConnection->Close(CTCPConnection::Error::Ok);
		connectionVars.m_pConnection = SysSmartPtr<CTCPConnection>();

		FixedString<SYSSOCKET_IPADDRESS_MAX_SIZE> strIP;
		connectionVars.m_connectionAddress.GetIPAddress(strIP.Buffer(), strIP.Size());

		FixedString<EVENTLOG_DETAILS_MAX_SIZE> strDetailsText;
		strDetailsText = strIP;
		strDetailsText += " disconnected.";

		CEventLogSQLTable eventLog(gDatabase);
		CEventLogEvent inEvent("Connection", strDetailsText.ConstBuffer());
		eventLog.AddEvent(inEvent);
	
	}

	return eError;
}


CServerCli::Error::Enum CServerCli::Update(void)
{
//	SysDebugPrintf("cli update %d\n", m_eState);

	switch (m_eState)
	{
		case State::Disconnected:
		{
			//-- Nothing to do.
		}
		break;

		case State::Connected:
		case State::LoggedIn:
		case State::Disconnecting:
		default:
		{
			if (IS_PTR(m_pConnection.ptr()))
			{
				CTCPConnection::Error::Enum eConRecvEr = m_pConnection->UpdateRecv(m_recvMessageList, m_sendMessageList);
				switch (eConRecvEr)
				{
					case CTCPConnection::Error::Ok:
					{
						//-- Nothing to do.
					}
					break;
					case CTCPConnection::Error::Closed:
					{
						//-- Socket was closed nicely.
						return Reset(Error::ConnectionClosed);
					}
					break;
					default:
					{
						//-- Some other kind of connection error.
						return Reset(Error::ConnectionError);
					}
					break;
				}
			}

			Error::Enum eNotifyError = NotifyRecvMessageHandlers();
			if (Error::Ok != eNotifyError)
			{
				return Reset(eNotifyError);
			}

			if (IS_PTR(m_pConnection.ptr()))
			{
				CTCPConnection::Error::Enum eConSendEr = m_pConnection->UpdateSend(m_sendMessageList);
				switch (eConSendEr)
				{
					case CTCPConnection::Error::Ok:
					{
						//-- Nothing to do.
					}
					break;
					default:
					{
						//-- Some kind of connection error.
						return Reset(Error::ConnectionError);
					}
					break;
				}
			}
		}
	}

	return Error::Ok;
}


s32 CServerCli::ByeHandler(SysSmartPtr<CMessage> pMessage, void* pData, bool bAlreadyHandled)
{
	CMessage::Type nType = pMessage->GetType();
	assert(CMsgBye::kType == nType);

	if (CMsgBye::kType == nType)
	{
		if (IS_NULL_PTR(pData))
		{
			return -1;
		}
			
		//-- Any Bye received will already have been acknowledged and responded to internally.
		//-- This is just in case we want to take any action on receiving it.
		//-- (which is no action at the moment.)
		//-- Some time after receiving MsgBye, the connection UpdateRecv() function will return Error::Closed,
		//-- and _that_ is the correct place to handle cleanup, _not_ here.
		return 1;
	}

	return 0;
}


s32 CServerCli::ClientStateChangeHandler(SysSmartPtr<CMessage> pMessage, void* pData, bool bAlreadyHandled)
{
	CMessage::Type nType = pMessage->GetType();

	SysDebugPrintf("ClientStateChangeHandler %s\n", CMessageFactory::GetTypeString(nType));	

	switch (nType)
	{
		case CMsgLoginRequest::kType:
		{
			CServerCli* _this = (CServerCli*)pData;
			if (IS_NULL_PTR(pData))
			{
				return -1;
			}

			assert(State::Connected == _this->m_eState);
			assert(IS_FALSE(bAlreadyHandled));

			if (State::Connected != _this->m_eState)
			{	
				return -1;
			}
			
			CMessage* pM = pMessage.ptr();
			CMsgLoginRequest* pLogin = (CMsgLoginRequest*)pM;

			SysDebugPrintf("%s - %s\n", pLogin->GetName(), pLogin->GetPassword());	

			FixedString<SYSSOCKET_IPADDRESS_MAX_SIZE> strIP;
			_this->m_connectionAddress.GetIPAddress(strIP.Buffer(), strIP.Size());

			FixedString<256> strEventMessage;
			strEventMessage = "USER ";
			strEventMessage += pLogin->GetName();
			strEventMessage += " PASS ";
			strEventMessage += pLogin->GetPassword();
			strEventMessage += " from ";
			strEventMessage += strIP;

			CEventLogSQLTable eventLog(gDatabase);
			CUsersSQLTable usersTable(gDatabase);
			CUserRecord user(pLogin->GetName(), pLogin->GetPassword(), strIP.ConstBuffer());
			switch (usersTable.Login(user))
			{
				case CUsersSQLTable::Error::Ok:
				{
					if (Error::Ok == _this->SendMessage(SysSmartPtr<CMessage>(new CMsgLoginResponse())))
					{
						strEventMessage += " logged in successfully.";
						CEventLogEvent inEvent("Login Success", strEventMessage.ConstBuffer());
						eventLog.AddEvent(inEvent);
						_this->m_eState = State::LoggedIn;
						return 1;
					}
					else
					{
						return -1;
					}
				}
				break;

				case CUsersSQLTable::Error::NoSuchUser:
				{
					strEventMessage += " ";
					if (Error::Ok == _this->SendMessage(SysSmartPtr<CMessage>(new CMsgBye(CMsgBye::Reason::LoginNoSuchUser))))
					{
						strEventMessage += " failed login: No such user.";
						CEventLogEvent inEvent("Login Failed", strEventMessage.ConstBuffer());
						eventLog.AddEvent(inEvent);
						return 1;
					}
					else
					{
						return -1;
					}
				}
				break;

				case CUsersSQLTable::Error::IncorrectPassword:
				{
					if (Error::Ok == _this->SendMessage(SysSmartPtr<CMessage>(new CMsgBye(CMsgBye::Reason::LoginIncorrectPassword))))
					{
						strEventMessage += " failed login: Incorrect password.";
						CEventLogEvent inEvent("Login Failed", strEventMessage.ConstBuffer());
						eventLog.AddEvent(inEvent);
						return 1;
					}
					else
					{
						return -1;
					}
				}
				break;
				
				default:
				{
					return -1;
				}
				break;
			}
		}
		break;

		default:
		break;
	}

	return 0;
}


//-- Trigger a nice disconnect.
CServerCli::Error::Enum CServerCli::Disconnect(void)
{
	//-- Simply add a CMsgBye(Reason::ServerDisconnected) to the sendList.
	//-- CTCPConnection will do the rest.

	SysSmartPtr<CMessage> pMessage(new CMsgBye(CMsgBye::Reason::ServerDisconnected));
	CMsgBye* pBye = (CMsgBye*)pMessage.ptr();
	if (IS_PTR(pBye))
	{
		return SendMessage(pMessage);
	}

	return Reset(Error::BadFail);
}


CServerCli::Error::Enum CServerCli::AddRecvMessageHandler(CMessage::Type nType, TRecvMessageHandlerFunc handlerFunc, void* pData)
{
	m_RecvMessageHandlerArray[nType].push_back(RecvMessageHandlerRecord(handlerFunc, pData));

	return Error::Ok;
}


CServerCli::Error::Enum CServerCli::RemoveRecvMessageHandler(CMessage::Type nType, TRecvMessageHandlerFunc handlerFunc)
{
	TRecvMessageHandlerRecordList recordList = m_RecvMessageHandlerArray[nType];

	for (TRecvMessageHandlerRecordList::iterator it = recordList.begin(); it != recordList.end(); ++it)
	{
		if (it->m_pCB == handlerFunc)
		{
			recordList.erase(it);
			return Error::Ok;
		}
	}

	return Error::NotRegistered;
}


CServerCli::Error::Enum CServerCli::NotifyRecvMessageHandlers(void)
{
	while (m_recvMessageList.size() > 0)
	{
		SysSmartPtr<CMessage> pMessage = m_recvMessageList.front();
		CMessage::Type nType = pMessage->GetType();

		TRecvMessageHandlerRecordList recordList = m_RecvMessageHandlerArray[nType];
		bool bAlreadyProcessed = false;

		for (TRecvMessageHandlerRecordList::iterator it = recordList.begin(); it != recordList.end(); ++it)
		{
			s32 nRes = it->m_pCB(pMessage, it->m_pData, bAlreadyProcessed);
			if (nRes > 0)
			{			
				bAlreadyProcessed = true;
			}
			else if (nRes < 0)
			{
				return Error::BadFail;
			}
		}

		m_recvMessageList.pop_front();
	}

	return Error::Ok;
}


CServerCli::Error::Enum CServerCli::SendMessage(SysSmartPtr<CMessage> pMessage)
{
	if (IS_PTR(pMessage.ptr()))
	{
		m_sendMessageList.push_back(pMessage);		
		return Error::Ok;
	}

	return Error::NullMessage;
}






//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//
