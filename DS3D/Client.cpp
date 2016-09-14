//----------------------------------------------------------//
// CLIENT.CPP
//----------------------------------------------------------//
//-- Description
// Client connection wrapper. Contains a TCP connection,
// message queues and utilities for handling incoming/outgoing
// messages.
//----------------------------------------------------------//


#include "Client.h"
#include "SystemIncludes.h"
#include "TCPConnection.h"
#include "TCPNonBlockingConnector.h"
#include "Network.h"
#include "Message.h"

#include "Messages/MsgBye.h"
#include "Messages/MsgServerKeyExchange.h"
#include "Messages/MsgLoginRequest.h"
#include "Messages/MsgLoginResponse.h"

#include "Ini.h"


//----------------------------------------------------------//
// DEFINES
//----------------------------------------------------------//

//----------------------------------------------------------//
// GLOBALS
//----------------------------------------------------------//


CClient::CClient()
: m_eState(State::Disconnected)
{
	//-- Just touch these, otherwise they never get added to the CMessageFactory.
	CMsgLoginResponse dummy;

	m_RecvMessageHandlerArray.clear();

	Reset(Error::Ok);
}


CClient::~CClient()
{
	Shutdown();
}


CClient::Error::Enum CClient::Initialise(void)
{
	m_RecvMessageHandlerArray.clear();
	//-- Register a handler to process incoming messages related to logging in.
	AddRecvMessageHandler(CMsgServerKeyExchange::kType, &CClient::ClientStateChangeHandler, this);
	AddRecvMessageHandler(CMsgLoginResponse::kType, &CClient::ClientStateChangeHandler, this);
	//-- Register a handler to process incoming CMsgBye messages
	AddRecvMessageHandler(CMsgBye::kType, &CClient::ByeHandler, this);

	return Reset(Error::Ok);
}


CClient::Error::Enum CClient::Shutdown(void)
{
	//-- Not a nice disconnect!
	m_RecvMessageHandlerArray.clear();

	return Reset(Error::Ok);
}


CClient::Error::Enum CClient::Reset(CClient::Error::Enum eError)
{
	m_eState = State::Disconnected;

	m_recvMessageList.clear();
	m_sendMessageList.clear();

	if (IS_PTR(m_pConnection.get()))
	{
		m_pConnection->Close(CTCPConnection::Error::Ok);
		m_pConnection = SysSmartPtr<CTCPConnection>();
	}

	return eError;
}


CClient::Error::Enum CClient::Update(void)
{
	switch (m_eState)
	{
		case State::Disconnected:
		{
			//-- Nothing to do.
		}
		break;

		case State::Connecting:
		{
			//-- State::Connecting means we're waiting for CTCPNonblockingConnector.
			CTCPConnector::Result r = m_connector.Update();
			switch (r.m_eError)
			{
				case CTCPConnector::Error::Ok:
				{
					m_pConnection = r.m_pConnection;
					m_eState = State::Connected;
				}
				break;
				case CTCPConnector::Error::InProgress:
				{
					//-- Still working. Do nothing.
				}
				break;
				default:
				{
					//-- Connect error occured.
					return Reset(Error::ConnectFailed);
				}
				break;
			}
		}
		break;

		case State::Connected:
		case State::LoggingIn:
		case State::LoggedIn:
		case State::Disconnecting:
		default:
		{
			if (IS_PTR(m_pConnection.get()))
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

			if (IS_PTR(m_pConnection.get()))
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


s32 CClient::ByeHandler(SysSmartPtr<CMessage> pMessage, void* pData, bool bAlreadyHandled)
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


s32 CClient::ClientStateChangeHandler(SysSmartPtr<CMessage> pMessage, void* pData, bool bAlreadyHandled)
{
	CMessage::Type nType = pMessage->GetType();

	switch (nType)
	{
		case CMsgServerKeyExchange::kType:
		{
			CClient* _this = (CClient*)pData;
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

			//-- We can progress to logging in.
			CIni ini;
			ini.Initialise();
			if (IS_FALSE(ini.Load("Data/config.ini")))
			{
				gDebugLog.Printf("Failed to open config.ini.");
				return -1;
			}

			SysSmartPtr<CMessage> pMessage(new CMsgLoginRequest());
			CMsgLoginRequest* pLogin = (CMsgLoginRequest*)pMessage.get();
			if (IS_NULL_PTR(pLogin))
			{
				return -1;
			}

			pLogin->SetName(ini.ReadString("Network", "Email"));
			pLogin->SetPassword(ini.ReadString("Network", "Password"));

			if (Error::Ok != _this->SendMessage(pMessage))
			{
				return -1;
			}
			
			_this->m_eState = State::LoggingIn;
			return 1;
		}
		break;

		case CMsgLoginResponse::kType:
		{
			CClient* _this = (CClient*)pData;
			if (IS_NULL_PTR(pData))
			{
				return -1;
			}

			assert(State::LoggingIn == _this->m_eState);
			assert(IS_FALSE(bAlreadyHandled));

			if (State::LoggingIn != _this->m_eState)
			{	
				return -1;
			}

			//-- We can progress to logged in.
			_this->m_eState = State::LoggedIn;
			return 1;
		}
		break;

		default:
		break;
	}

	return 0;
}


CClient::Error::Enum CClient::Connect(s8* strServer)
{
	if (State::Disconnected != m_eState)
	{
		return Error::WrongState;
	}

	s8* pNext;
	s8* strIP = SysString::Strtok(strServer, ":", pNext);
	s8* strPort = SysString::Strtok(NULL, "\r\n", pNext);
	if ( IS_TRUE(SysString::IsEmpty(strIP))
		|| IS_TRUE(SysString::IsEmpty(strPort)) )
	{
		return Error::BadServerAddress;
	}

	return Connect(strIP, strPort);
}


CClient::Error::Enum CClient::Connect(const s8* strIP, const s8* strPort)
{
	if (State::Disconnected != m_eState)
	{
		return Error::WrongState;
	}

	CTCPConnector::Error::Enum eE = m_connector.Connect(strIP, strPort);
	if (CTCPConnector::Error::InProgress != eE)
	{
		//-- Error occured.
		return Reset(Error::ConnectFailed);
	}

	m_eState = State::Connecting;

	return Error::Ok;
}


//-- Trigger a nice disconnect.
CClient::Error::Enum CClient::Disconnect(void)
{
	//-- Simply add a CMsgBye(Reason::ClientDisconnected) to the sendList.
	//-- CTCPConnection will do the rest.

	SysSmartPtr<CMessage> pMessage(new CMsgBye(CMsgBye::Reason::ClientDisconnected));
	CMsgBye* pBye = (CMsgBye*)pMessage.get();
	if (IS_PTR(pBye))
	{
		return SendMessage(pMessage);
	}

	return Reset(Error::BadFail);
}


CClient::Error::Enum CClient::AddRecvMessageHandler(CMessage::Type nType, TRecvMessageHandlerFunc handlerFunc, void* pData)
{
	m_RecvMessageHandlerArray[nType].push_back(RecvMessageHandlerRecord(handlerFunc, pData));

	return Error::Ok;
}


CClient::Error::Enum CClient::RemoveRecvMessageHandler(CMessage::Type nType, TRecvMessageHandlerFunc handlerFunc)
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


CClient::Error::Enum CClient::NotifyRecvMessageHandlers(void)
{
	while (m_recvMessageList.size() > 0)
	{
		SysSmartPtr<CMessage> pMessage = m_recvMessageList.front();
		CMessage::Type nType = pMessage->GetType();

		TRecvMessageHandlerRecordList recordList = m_RecvMessageHandlerArray[nType];
		bool bAlreadyProcessed = false;

		for (TRecvMessageHandlerRecordList::iterator it = recordList.begin(); it != recordList.end(); ++it)
		{
			if (IS_PTR(it->m_pCB))
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
		}

		m_recvMessageList.pop_front();
	}

	return Error::Ok;
}


CClient::Error::Enum CClient::SendMessage(SysSmartPtr<CMessage> pMessage)
{
	if (IS_PTR(pMessage.get()))
	{
		m_sendMessageList.push_back(pMessage);		
		return Error::Ok;
	}

	return Error::NullMessage;
}


//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//
