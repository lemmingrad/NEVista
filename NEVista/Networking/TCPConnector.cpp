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


//----------------------------------------------------------//
// DEFINES
//----------------------------------------------------------//

//----------------------------------------------------------//
// GLOBALS
//----------------------------------------------------------//

//----------------------------------------------------------//
// CTCPConnector::CTCPConnector
//----------------------------------------------------------//
//-- Description			
//----------------------------------------------------------//
CTCPConnector::CTCPConnector()
: m_eState(State::NotConnected)
, m_pAddrResults(NULL)
, m_pCur(NULL)
, m_nSocket(SysSocket::INVALID_SOCK)
{
}


//----------------------------------------------------------//
// CTCPConnector::~CTCPConnector
//----------------------------------------------------------//
//-- Description			
//----------------------------------------------------------//
CTCPConnector::~CTCPConnector()
{
	CleanAddrResults();
}


//----------------------------------------------------------//
// CTCPConnector::CleanAddrResults
//----------------------------------------------------------//
//-- Description			
//----------------------------------------------------------//
void CTCPConnector::CleanAddrResults(void)
{
	if (IS_PTR(m_pAddrResults))
	{
		SysSocket::FreeInfo(m_pAddrResults);
		m_pAddrResults = NULL;
		m_pCur = NULL;
	}
}


//----------------------------------------------------------//
// CTCPConnector::CleanSocket
//----------------------------------------------------------//
//-- Description			
//----------------------------------------------------------//
void CTCPConnector::CleanSocket(void)
{
	if (SysSocket::INVALID_SOCK != m_nSocket)
	{
		SysSocket::CloseSocket(m_nSocket);
		m_nSocket = SysSocket::INVALID_SOCK;
	}
}


//----------------------------------------------------------//
// EXTERNALS
//----------------------------------------------------------//

//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//
