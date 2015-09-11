//----------------------------------------------------------//
// TCPNONBLOCKINGACCEPTOR.CPP
//----------------------------------------------------------//
//-- Description			
// Establishes a TCP listener on a sevrer end, handles new
// incoming connection. 
// Asynchronous non-blocking acceptor.
//----------------------------------------------------------//


#include "TCPNonBlockingAcceptor.h"
#include "TCPAcceptor.h"
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
// CTCPNonblockingAcceptor::CTCPNonblockingAcceptor
//----------------------------------------------------------//
//-- Description			
//----------------------------------------------------------//
CTCPNonblockingAcceptor::CTCPNonblockingAcceptor()
: CTCPAcceptor()
{
	m_pollFd.fd = SysSocket::INVALID_SOCK;
	m_pollFd.events = POLLIN;
}


//----------------------------------------------------------//
// CTCPNonblockingAcceptor::~CTCPNonblockingAcceptor
//----------------------------------------------------------//
//-- Description			
//----------------------------------------------------------//
CTCPNonblockingAcceptor::~CTCPNonblockingAcceptor()
{
	StopListening();
}


//----------------------------------------------------------//
// CTCPNonblockingAcceptor::Listen
//----------------------------------------------------------//
//-- Description			
// Configures a socket for listening.
//----------------------------------------------------------//
CTCPAcceptor::Error::Enum CTCPNonblockingAcceptor::Listen(const s8* strPort, s32 nBacklog)
{
	if (State::NotListening != m_eState)
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
	hints.ai_flags = AI_PASSIVE;

	if (SYS_SOCKET_NO_ERROR == SysSocket::GetInfo(NULL, strPort, &hints, &m_pAddrResults))
	{
		eError = Error::InProgress;
		m_eState = State::GettingAddrInfo;
		m_nBacklog = nBacklog;
	}
	else
	{
		eError = Error::GetInfoFail;
		m_eState = State::NotListening;
	}

	return eError;
}


//----------------------------------------------------------//
// CTCPNonblockingAcceptor::Accept
//----------------------------------------------------------//
//-- Description			
// Updates internal state and calls accept() as necessary.
// Note that the result contains a NON-BLOCKING CTCPConnection.
//----------------------------------------------------------//
CTCPAcceptor::Result CTCPNonblockingAcceptor::Accept(void)
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

			if (SYS_SOCKET_NO_ERROR != SysSocket::SetReusable(m_nSocket, true))
			{		
				eError = Error::SetReusableFail;
				break;
			}

			if (SYS_SOCKET_NO_ERROR != SysSocket::Bind(m_nSocket, m_pCur->ai_addr, m_pCur->ai_addrlen))
			{
				eError = Error::BindFail; 
				break;
			}

			if (SYS_SOCKET_NO_ERROR == SysSocket::Listen(m_nSocket, m_nBacklog))
			{
				eError = Error::ListenFail;
				break;
			}

			//-- Success.
			m_pollFd.fd = m_nSocket;
			m_eState = State::Listening;
		}
		break;

		case State::Listening:
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
	
			if (TEST_FLAG(m_pollFd.revents, POLLIN))
			{
				//-- Ready to accept
				eError = Error::Ok;
			}
			if (TEST_FLAG(m_pollFd.revents, POLLERR))
			{
				//-- Error.
				eError = Error::SelectFail;
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
			size_t nSize = sizeof(result.m_Addr);

			SysSocket::Socket nNewsock = SysSocket::Accept(m_nSocket, (SysSocket::SockAddr*)&result.m_Addr, &nSize);
			if (SysSocket::INVALID_SOCK != nNewsock)
			{
				result.m_pConnection = SysSmartPtr<CTCPConnection>(new CTCPConnection(nNewsock));
			}
			result.m_eError = Error::Ok;

			CleanAddrResults();
			m_eState = State::Listening;
		}
		break;

		case Error::WrongState:
		{
			result.m_eError = Error::WrongState;

			CleanAddrResults();
			CleanSocket();
			m_pollFd.fd = SysSocket::INVALID_SOCK;

			m_eState = State::NotListening;
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
					default:
					{
						//-- Unexpected state!
						//-- This is a bad failure and will stop the process.
						result.m_eError = eError;

						CleanAddrResults();
						m_eState = State::NotListening;
					}
					break;
				}
			}
			else
			{
				//-- No more addresses to try.
				result.m_eError = eError;

				CleanAddrResults();
				m_eState = State::NotListening;
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
