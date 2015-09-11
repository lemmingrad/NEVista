//----------------------------------------------------------//
// TCPCONNECTOR.CPP
//----------------------------------------------------------//
//-- Description			
// Establishes a TCP connection from a client end.
// BLOCKING.
//----------------------------------------------------------//


#include "TCPBlockingConnector.h"
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
// CTCPBlockingConnector::CTCPBlockingConnector
//----------------------------------------------------------//
//-- Description			
//----------------------------------------------------------//
CTCPBlockingConnector::CTCPBlockingConnector()
: CTCPConnector()
{
}


//----------------------------------------------------------//
// CTCPBlockingConnector::~CTCPBlockingConnector
//----------------------------------------------------------//
//-- Description			
//----------------------------------------------------------//
CTCPBlockingConnector::~CTCPBlockingConnector()
{
	CleanAddrResults();
}


//----------------------------------------------------------//
// CTCPBlockingConnector::Connect
//----------------------------------------------------------//
//-- Description	
// Calls connect() and blocks until complete.
// Note that the returned result contains a BLOCKING
// CTCPConnection.
//----------------------------------------------------------//
CTCPConnector::Result CTCPBlockingConnector::Connect(const s8* strHostname, const s8* strPort)
{
	Result result;

	if (State::NotConnected != m_eState)
	{
		result.m_eError = Error::WrongState;
		return result;
	}

	CleanAddrResults();
	CleanSocket();

	SysSocket::AddrInfo hints;
	SysMemory::Memclear(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	if (SYS_SOCKET_NO_ERROR != SysSocket::GetInfo(strHostname, strPort, &hints, &m_pAddrResults))
	{
		result.m_eError = Error::GetInfoFail;
		return result;
	}

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


//----------------------------------------------------------//
// EXTERNALS
//----------------------------------------------------------//

//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//
	
