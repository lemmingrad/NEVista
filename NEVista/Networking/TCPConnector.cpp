//----------------------------------------------------------//
// TCPCONNECTOR.CPP
//----------------------------------------------------------//
//-- Description			
// Establishes a TCP connection from a client end.
//----------------------------------------------------------//

#include "TCPConnector.h"
#include "Types.h"
#include "SysSocket.h"
#include "TCPConnection.h"



CTCPConnector::CTCPConnector()
: m_eState(State::NotConnected)
, m_pAddrResults(NULL)
, m_pCur(NULL)
, m_nSocket(SysSocket::INVALID_SOCK)
{
}


CTCPConnector::~CTCPConnector()
{
	CleanAddrResults();
}


void CTCPConnector::CleanAddrResults(void)
{
	if (IS_PTR(m_pAddrResults))
	{
		SysSocket::FreeInfo(m_pAddrResults);
		m_pAddrResults = NULL;
		m_pCur = NULL;
	}
}


void CTCPConnector::CleanSocket(void)
{
	if (SysSocket::INVALID_SOCK != m_nSocket)
	{
		SysSocket::CloseSocket(m_nSocket);
		m_nSocket = SysSocket::INVALID_SOCK;
	}
}


CTCPConnector::Result CTCPConnector::ConnectBlocking(const s8* strHostname, const s8* strPort)
{
	CleanAddrResults();
	CleanSocket();

	Result result;

	if (State::NotConnected != m_eState)
	{
		result.m_eError = Error::WrongState;
		return result;
	}

	SysSocket::AddrInfo hints;
	SysMemory::Memclear(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	if (SYS_SOCKET_NO_ERROR != SysSocket::GetInfo(strHostname, strPort, &hints, &m_pAddrResults))
	{
		result.m_eError = Error::GetInfoFail;
		return result;
	}

	m_nSocket = SysSocket::INVALID_SOCK;

	for (m_pCur = m_pAddrResults; IS_PTR(m_pCur); m_pCur = m_pCur->ai_next)
	{
		m_nSocket = SysSocket::OpenSocket(m_pCur->ai_family, m_pCur->ai_socktype, m_pCur->ai_protocol);
		if (SysSocket::INVALID_SOCK == m_nSocket)
		{
			result.m_eError = Error::OpenFail;
			continue;
		}

		//-- Success
		if (SYS_SOCKET_NO_ERROR == SysSocket::Connect(m_nSocket, m_pCur->ai_addr, m_pCur->ai_addrlen))
		{
			//-- Successfully opened and connected, so break out of the loop.
			result.m_eError = Error::Ok;
			break;
		}

		//-- We failed to connect, but succeeded at open, so we must close before we
		//-- try to open a new socket.
		CleanSocket();
		result.m_eError = Error::ConnectFail;
	}

	if ( (Error::Ok == result.m_eError)
		&& (SysSocket::INVALID_SOCK != m_nSocket) )
	{
		//-- Success
		m_eState = State::Connected;
		result.m_pConnection = new CTCPConnection(m_nSocket);
	}
	else
	{
		//-- Failed
		m_eState = State::NotConnected;
		CleanSocket();
	}

	CleanAddrResults();

	return result;
}
	

CTCPConnector::Error::Enum CTCPConnector::ConnectNonblocking(const s8* strHostname, const s8* strPort)
{
	CleanAddrResults();
	CleanSocket();
	
	Error::Enum eError;
	SysSocket::AddrInfo hints;

	SysMemory::Memclear(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	if (SYS_SOCKET_NO_ERROR == SysSocket::GetInfo(strHostname, strPort, &hints, &m_pAddrResults))
	{
		eError = Error::InProgress;
		m_eState = State::GettingAddrInfo;
	}
	else
	{
		eError = Error::GetInfoFail;
		m_eState = State::NotConnected;
	}

	return eError;
}


CTCPConnector::Result CTCPConnector::Update(void)
{
	Result result;
	Error::Enum eError = Error::InProgress;

	switch (m_eState)
	{
		default:
		{
			eError = Error::WrongState;
		}
		break;

		case State::GettingAddrInfo:
		{
			m_pCur = m_pAddrResults;
			m_eState = State::NextAddr;
		}
		break;

		case State::NextAddr:
		{
			if (IS_NULL_PTR(m_pCur))
			{
				eError = Error::NoMoreInfo;
				break;
			}

			m_nSocket = SysSocket::OpenSocket(m_pCur->ai_family, m_pCur->ai_socktype, m_pCur->ai_protocol);
			if (SysSocket::INVALID_SOCK == m_nSocket)
			{
				eError = Error::OpenFail;
				break;
			}

			if (SYS_SOCKET_NO_ERROR != SysSocket::SetNonblocking(m_nSocket, true))
			{
				eError = Error::SetSockOptFail;
				break;
			}

			//-- Success
			switch (SysSocket::Connect(m_nSocket, m_pCur->ai_addr, m_pCur->ai_addrlen))
			{
				case SYS_SOCKET_IN_PROGRESS:
				{
					//-- Expected return value for a non-blocking connect
					m_eState = State::Connecting;
				}
				break;
				case SYS_SOCKET_NO_ERROR:
				{
					//-- Connected immediately
					eError = Error::Ok;
				}
				break;
				default:
				{
					eError = Error::ConnectFail;
				}
				break;
			}
		}
		break;

		case State::Connecting:
		{
			//-- Wait for select
			SysSocket::FdSet fdset;
			FD_ZERO(&fdset);
			FD_SET(m_nSocket, &fdset);
			SysSocket::Timeval tv = {0};

			s32 nError = SysSocket::Select(m_nSocket + 1, NULL, &fdset, NULL, &tv);
			if (SYS_SOCKET_ERROR == nError)
			{
				//-- Error occurred in select.
				eError = Error::SelectFail;
				break;
			}
	
			if (FD_ISSET(m_nSocket, &fdset))
			{
				s32 nVal;
				size_t nValSize = sizeof(nVal);
				s32 nErr = SysSocket::GetSockOpt(m_nSocket, SOL_SOCKET, SO_ERROR, &nVal, &nValSize); 
				if (SYS_SOCKET_ERROR == nError)
				{
					//-- Error.
					eError = Error::GetSockOptFail;
					break;
				}

				if (SYS_SOCKET_NO_ERROR == nVal)
				{
					//-- Connected
					eError = Error::Ok;
				}
				else
				{
					//-- Some other kind of error on socket.
					eError = Error::ConnectFail;
				}
			}
		}
		break;
	}

	switch (eError)
	{
		case Error::InProgress:
		{
			//-- Nothing to do
			result.m_eError = Error::InProgress;
		}
		break;

		case Error::Ok:
		{
			//-- Finished!
			if (SysSocket::INVALID_SOCK != m_nSocket)
			{
				result.m_pConnection = new CTCPConnection(m_nSocket);
			}
			result.m_eError = Error::Ok;

			CleanAddrResults();
			m_eState = State::Connected;
		}
		break;

		case Error::WrongState:
		{
			result.m_eError = Error::WrongState;

			CleanSocket();
			CleanAddrResults();
			m_eState = State::NotConnected;
		}
		break;

		default:
		{
			//-- Any other kind of error.
			CleanSocket();

			if (IS_PTR(m_pCur))
			{
				m_pCur = m_pCur->ai_next;

				switch (m_eState)
				{
					case State::NextAddr:
					{
						//-- return error as still 'in progress' instead of actual error.
						result.m_eError = Error::InProgress;
					}
					break;
					case State::Connecting:
					{
						//-- return error as still 'in progress' instead of actual error.
						result.m_eError = Error::InProgress;
						//-- reset state to loop onto next next address
						m_eState = State::NextAddr;
					}
					break;
					default:
					{
						//-- Unexpected state!
						//-- This is a bad failure and will stop the connect process.
						result.m_eError = eError;

						CleanAddrResults();
						m_eState = State::NotConnected;
					}
					break;
				}
			}
			else
			{
				//-- No more addresses to try.
				result.m_eError = eError;

				CleanAddrResults();
				m_eState = State::NotConnected;
			}
		}
	}

	return result;
}
