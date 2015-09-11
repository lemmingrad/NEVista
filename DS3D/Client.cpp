//----------------------------------------------------------//
// GAME.CPP
//----------------------------------------------------------//
//-- Description
// Main game initialise/shutdown/loop and main()
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


CClient::Error::Enum CClient::Reset(CClient::Error::Enum eError)
{
	m_eState = State::Disconnected;

	m_recvMessageList.clear();
	m_sendMessageList.clear();

	if (IS_PTR(m_pConnection.ptr()))
	{
		m_pConnection->Close(CTCPConnection::Error::Ok);
		m_pConnection = SysSmartPtr<CTCPConnection>();
	}

	return eError;
}


CClient::Error::Enum CClient::Initialise(void)
{
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


bool CClient::ByeHandler(CMessage::Type nType, void* pData, bool bAlreadyHandled)
{
	assert(CMsgBye::kType == nType);

	if (CMsgBye::kType == nType)
	{
		if (IS_NULL_PTR(pData))
		{
			return false;
		}
			
		//-- Any Bye received will already have been acknowledged and responded to internally.
		//-- This is just in case we want to take any action on receiving it.
		//-- (which is no action at the moment.)
		//-- Some time after receiving MsgBye, the connection UpdateRecv() function will return Error::Closed,
		//-- and _that_ is the correct place to handle cleanup, _not_ here.
		return false;
	}

	return false;
}


bool CClient::ClientStateChangeHandler(CMessage::Type nType, void* pData, bool bAlreadyHandled)
{
	switch (nType)
	{
		case CMsgServerKeyExchange::kType:
		{
			CClient* _this = (CClient*)pData;
			if (IS_NULL_PTR(pData))
			{
				return false;
			}

			assert(State::Connected == _this->m_eState);
			assert(IS_FALSE(bAlreadyHandled));

			if (State::Connected == _this->m_eState)
			{			
				//-- We can progress to logging in.
				CMsgLoginRequest* pLogin = new CMsgLoginRequest();
				if (IS_PTR(pLogin))
				{
					_this->SendMessage(SysSmartPtr<CMessage>(pLogin));
					_this->m_eState = State::LoggingIn;

					return true;
				}
			}
		}
		break;

		case CMsgLoginResponse::kType:
		{
			CClient* _this = (CClient*)pData;
			if (IS_NULL_PTR(pData))
			{
				return false;
			}

			assert(State::LoggingIn == _this->m_eState);
			assert(IS_FALSE(bAlreadyHandled));

			if (State::LoggingIn == _this->m_eState)
			{	
				//-- We can progress to logged in.
				_this->m_eState = State::LoggedIn;

				return true;
			}
		}
		break;

		default:
		break;
	}

	return false;
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
	//-- Simply add a CMsgBye(Reason::NormalDisconnect) to the sendList.
	//-- CTCPConnection will do the rest.

	CMsgBye* pBye = new CMsgBye(CMsgBye::Reason::NormalDisconnect);
	if (IS_NULL_PTR(pBye))
	{
		return Reset(Error::BadFail);
	}

	return SendMessage(SysSmartPtr<CMessage>(pBye));
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

	return Error::NotFound;
}


CClient::Error::Enum CClient::NotifyRecvMessageHandlers(void)
{
	while (m_recvMessageList.size() > 0)
	{
		SysSmartPtr<CMessage>& pMessage = m_recvMessageList.front();
		CMessage::Type nType = pMessage->GetType();

		TRecvMessageHandlerRecordList recordList = m_RecvMessageHandlerArray[nType];
		bool bAlreadyProcessed = false;

		for (TRecvMessageHandlerRecordList::iterator it = recordList.begin(); it != recordList.end(); ++it)
		{
			if (IS_PTR(it->m_pCB))
			{
				bAlreadyProcessed |= it->m_pCB(nType, it->m_pData, bAlreadyProcessed);
			}
		}

		m_recvMessageList.pop_front();
	}

	return Error::Ok;
}


CClient::Error::Enum CClient::SendMessage(SysSmartPtr<CMessage> pMessage)
{
	m_sendMessageList.push_back(pMessage);		

	return Error::Ok;
}


//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//
