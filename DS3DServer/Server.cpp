//----------------------------------------------------------//
// SERVER.CPP
//----------------------------------------------------------//
//-- Description
// Wrapper for 'server' functionality.
// Contains a list of CServerCli objects representing
// all the connected clients.
//----------------------------------------------------------//


#include "Server.h"
#include "ServerCli.h"
#include "Types.h"
#include "SysDebugLog.h"

#include "SQLiteDatabase.h"
#include "EventLogSQLTable.h"


//----------------------------------------------------------//
// DEFINES
//----------------------------------------------------------//


#define SERVER_LISTENING_PORT 		"5555"
#define SERVER_LISTENING_BACKLOG 	10


//----------------------------------------------------------//
// GLOBALS
//----------------------------------------------------------//

extern CSQLiteDatabase gDatabase;



CServer::CServer()
: m_acceptor()
, m_eState(State::Idle)
{
	m_clientList.clear();
}

CServer::~CServer()
{
	Shutdown();
}


CServer::Error::Enum CServer::Initialise(void)
{
	m_clientList.clear();

	if (CTCPAcceptor::Error::InProgress != m_acceptor.Listen(SERVER_LISTENING_PORT, SERVER_LISTENING_BACKLOG))
	{
		Shutdown();
		return Error::AcceptorFailed;
	}

	m_eState = State::Listening;
	return Error::Ok;
}


CServer::Error::Enum CServer::Shutdown(void)
{
	//-- Nasty shutdown. Force-closes all connections immediately.
	for (TServerCliList::iterator it = m_clientList.begin(); it != m_clientList.end(); ++it)
	{
		(*it)->Shutdown();
	}
	m_clientList.clear();
	m_eState = State::Idle;

	return Error::Ok;
}


CServer::Error::Enum CServer::Takedown(u32 nTime)
{
//	SysSmartPtr<CMessage> pMessage(new CMsgChat(CMsgChat::Type::ServerAlert));
//	CMsgChat* pChat = (CMsgChat*)pMessage.ptr();
//	if (IS_PTR(pChat))
//	{
//		pChat->SetText("Server is going down NOW!");
//		BroadcastMessage(pMessage);
//	}

	for (TServerCliList::iterator it = m_clientList.begin(); it != m_clientList.end(); ++it)
	{
		(*it)->Disconnect();
	}

	m_eState = State::ShuttingDown;
	return Error::Ok;
}


CServer::Error::Enum CServer::Update(void)
{
	switch (m_eState)
	{
		case State::Idle:
		default:
		{
			//-- Nothing to do.
		}
		break;

		case State::Listening:
		{
			CTCPAcceptor::Result r = m_acceptor.Accept();
			switch (r.m_eError)
			{
				case CTCPAcceptor::Error::Ok:
				{
					if (IS_PTR(r.m_pConnection.ptr()))
					{
						SysSmartPtr<CServerCli> pNewServerCli(new CServerCli(*this));
						if (IS_PTR(pNewServerCli.ptr()))
						{
							CServerCli::Error::Enum eEr = pNewServerCli->Initialise(r.m_pConnection, r.m_address);
							if (CServerCli::Error::Ok == eEr)
							{
								FixedString<SYSSOCKET_IPADDRESS_MAX_SIZE> strIP;
								r.m_address.GetIPAddress(strIP.Buffer(), strIP.Size());
								SysDebugPrintf("acceptor new conn %s\n", strIP.ConstBuffer());

								m_clientList.push_back(pNewServerCli);
							}
						}
					}
				}
				break;
				case CTCPAcceptor::Error::InProgress:
				{
					//-- Still working. Do nothing.
				}
				break;
				default:
				{
					SysDebugPrintf("acceptor error 0x%x\n", (int)r.m_eError);
					//-- Acceptor error occured.
					return Shutdown();
				}
				break;
			}	

			for (TServerCliList::iterator it = m_clientList.begin(); it != m_clientList.end(); )
			{
				SysSmartPtr<CServerCli> pCli = *it;
				CServerCli::Error::Enum eError = pCli->Update();
				if (CServerCli::Error::Ok != eError)
				{
					pCli->Shutdown();
					it = m_clientList.erase(it);

					SysDebugPrintf("client error. Clis connected = %d\n", m_clientList.size());
				}
				else
				{
					++it;
				}
			}	
		}
		break;

		case State::ShuttingDown:
		{
			for (TServerCliList::iterator it = m_clientList.begin(); it != m_clientList.end(); )
			{
				SysSmartPtr<CServerCli> pCli = *it;
				CServerCli::Error::Enum eError = pCli->Update();
				if (CServerCli::Error::Ok != eError)
				{
					pCli->Shutdown();
					it = m_clientList.erase(it);
				}
				else
				{
					++it;
				}
			}	

			if (IS_ZERO(m_clientList.size()))
			{
				m_clientList.clear();
				m_eState = State::Idle;
			}
		}
		break;
	}

	return Error::Ok;
}


CServer::Error::Enum CServer::BroadcastMessage(SysSmartPtr<CMessage> pMessage, CServerCli* pException)
{
	if (IS_PTR(pException))
	{
		for (TServerCliList::iterator it = m_clientList.begin(); it != m_clientList.end(); ++it)
		{
			SysSmartPtr<CServerCli> pCli = *it;
			if (pCli.ptr() != pException)
			{
				pCli->SendMessage(pMessage);
			}
		}	
	}
	else
	{
		for (TServerCliList::iterator it = m_clientList.begin(); it != m_clientList.end(); ++it)
		{
			(*it)->SendMessage(pMessage);
		}	
	}

	return Error::Ok;
}

//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//
