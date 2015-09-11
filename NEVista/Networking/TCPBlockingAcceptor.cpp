//----------------------------------------------------------//
// TCPBLOCKINGACCEPTOR.CPP
//----------------------------------------------------------//
//-- Description			
// Establishes a TCP listener on a sevrer end, handles new
// incoming connection. BLOCKING.
//----------------------------------------------------------//


#include "TCPBlockingAcceptor.h"
#include "TCPAcceptor.h"
#include "Types.h"
#include "SysSocket.h"
#include "SysSmartPtr.h"
#include "TCPConnection.h"


//----------------------------------------------------------//
// DEFINES
//----------------------------------------------------------//

//----------------------------------------------------------//
// GLOBALS
//----------------------------------------------------------//

//----------------------------------------------------------//
// CTCPBlockingAcceptor::CTCPBlockingAcceptor
//----------------------------------------------------------//
//-- Description			
//----------------------------------------------------------//
CTCPBlockingAcceptor::CTCPBlockingAcceptor()
: CTCPAcceptor()
{
}


//----------------------------------------------------------//
// CTCPBlockingAcceptor::~CTCPBlockingAcceptor
//----------------------------------------------------------//
//-- Description			
//----------------------------------------------------------//
CTCPBlockingAcceptor::~CTCPBlockingAcceptor()
{
	StopListening();
}


//----------------------------------------------------------//
// CTCPBlockingAcceptor::Listen
//----------------------------------------------------------//
//-- Description	
// Configures a socket for listening.
//----------------------------------------------------------//
CTCPAcceptor::Error::Enum CTCPBlockingAcceptor::Listen(const s8* strPort, s32 nBacklog)
{
	if (State::NotListening != m_eState)
	{
		return Error::WrongState;
	}

	CleanAddrResults();
	CleanSocket();

	Error::Enum eError = Error::Ok;

	SysSocket::AddrInfo hints;
	SysMemory::Memclear(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	if (SYS_SOCKET_NO_ERROR != SysSocket::GetInfo(NULL, strPort, &hints, &m_pAddrResults))
	{
		return Error::GetInfoFail;
	}

	for (m_pCur = m_pAddrResults; IS_PTR(m_pCur); m_pCur = m_pCur->ai_next)
	{
		m_nSocket = SysSocket::OpenSocket(m_pCur->ai_family, m_pCur->ai_socktype, m_pCur->ai_protocol);
		if (SysSocket::INVALID_SOCK == m_nSocket)
		{
			eError = Error::OpenFail;
			continue;
		}

		//-- Enable reusable address
		if (SYS_SOCKET_NO_ERROR != SysSocket::SetReusable(m_nSocket, true))
		{
			//-- We failed to setreusable but succeeded at open, so we must close before we
			//-- try to open a new socket.
			eError = Error::SetReusableFail;
			CleanSocket();
			continue;
		}

		//-- Bind
		if (SYS_SOCKET_NO_ERROR != SysSocket::Bind(m_nSocket, m_pCur->ai_addr, m_pCur->ai_addrlen))
		{
			//-- We failed to bind but succeeded at open, so we must close before we
			//-- try to open a new socket.
			eError = Error::BindFail;
			CleanSocket();
			continue;
		}

		//-- Successful open, setreusable and bind, so break out of the loop.
		eError = Error::Ok;
		break;
	}

	CleanAddrResults();

	if (Error::Ok != eError)
	{
		//-- Failed
		m_eState = State::NotListening;
		CleanSocket();
		return eError;
	}

	if (SYS_SOCKET_ERROR == SysSocket::Listen(m_nSocket, nBacklog))
	{
		//-- Failed
		m_eState = State::NotListening;
		CleanSocket();
		return eError;
	}

	m_eState = State::Listening;
	return Error::Ok;
}


//----------------------------------------------------------//
// CTCPBlockingAcceptor::Accept
//----------------------------------------------------------//
//-- Description	
// Calls accept() and blocks until complete.
// Note that the result contains a BLOCKING CTCPConnection.
//----------------------------------------------------------//
CTCPAcceptor::Result CTCPBlockingAcceptor::Accept(void)
{
	Result result;

	if (State::Listening == m_eState)
	{
		size_t nSize = sizeof(result.m_Addr);

		SysSocket::Socket nNewsock = SysSocket::Accept(m_nSocket, (SysSocket::SockAddr*)&result.m_Addr, &nSize);
		if (SysSocket::INVALID_SOCK != nNewsock)
		{
			result.m_pConnection = SysSmartPtr<CTCPConnection>(new CTCPConnection(nNewsock));
		}

		result.m_eError = Error::Ok;
		return result;
	}
	else
	{
		m_eState = State::NotListening;
		CleanSocket();

		result.m_eError = Error::WrongState;
		return result;
	}
}


//----------------------------------------------------------//
// EXTERNALS
//----------------------------------------------------------//

//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//
