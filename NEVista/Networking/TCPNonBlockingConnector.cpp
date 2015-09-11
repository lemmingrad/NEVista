//----------------------------------------------------------//
// TCPNONBLOCKINGCONNECTOR.CPP
//----------------------------------------------------------//
//-- Description			
// Establishes a TCP connection from a client end.
// Asynchronous Non-blocking connector.
//----------------------------------------------------------//


#include "TCPNonBlockingConnector.h"
#include "TCPConnector.h"
#include "Types.h"
#include "SysSocket.h"
#include "TCPConnection.h"


//----------------------------------------------------------//
// DEFINES
//----------------------------------------------------------//

//----------------------------------------------------------//
// GLOBALS
//----------------------------------------------------------//

//----------------------------------------------------------//
// CTCPNonblockingConnector::CTCPNonblockingConnector
//----------------------------------------------------------//
//-- Description			
//----------------------------------------------------------//
CTCPNonblockingConnector::CTCPNonblockingConnector()
: CTCPConnector()
{
	m_pollFd.fd = SysSocket::INVALID_SOCK;
	m_pollFd.events = POLLOUT;
}


//----------------------------------------------------------//
// CTCPNonblockingConnector::~CTCPNonblockingConnector
//----------------------------------------------------------//
//-- Description			
//----------------------------------------------------------//
CTCPNonblockingConnector::~CTCPNonblockingConnector()
{
	CleanAddrResults();
}


//----------------------------------------------------------//
// CTCPNonblockingConnector::Connect
//----------------------------------------------------------//
//-- Description			
//----------------------------------------------------------//
CTCPConnector::Error::Enum CTCPNonblockingConnector::Connect(const s8* strHostname, const s8* strPort)
{
	if (State::NotConnected != m_eState)
	{
		return Error::WrongState;
	}

	CleanAddrResults();
	CleanSocket();
	m_pollFd.fd = SysSocket::INVALID_SOCK;
	
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


//----------------------------------------------------------//
// CTCPNonblockingConnector::Update
//----------------------------------------------------------//
//-- Description	
// Update the connector, opening a socket, calling 
// connect(), etc. 
// Note that the result contains a Non-blocking
// CTCPConnection.
//----------------------------------------------------------//
CTCPConnector::Result CTCPNonblockingConnector::Update(void)
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
					m_pollFd.fd = m_nSocket;
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
			//-- Wait for select/poll
			if (SysSocket::INVALID_SOCK == m_pollFd.fd)
			{
				break;
			}

			s32 nError = SysSocket::Poll(&m_pollFd, 1, 0);
			if (SYS_SOCKET_ERROR == nError)
			{
				//-- Error occurred in select.
				eError = Error::SelectFail;
				break;
			}
	
			if (TEST_FLAG(m_pollFd.revents, POLLOUT))
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
			if (TEST_FLAG(m_pollFd.revents, POLLERR))
			{
				//-- Error.
				eError = Error::ConnectFail;
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

			CleanAddrResults();
			CleanSocket();
			m_pollFd.fd = SysSocket::INVALID_SOCK;

			m_eState = State::NotConnected;
		}
		break;

		default:
		{
			//-- Any other kind of error.
			CleanSocket();
			m_pollFd.fd = SysSocket::INVALID_SOCK;

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


//----------------------------------------------------------//
// EXTERNALS
//----------------------------------------------------------//

//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//
