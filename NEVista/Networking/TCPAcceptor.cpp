//----------------------------------------------------------//
// TCPACCEPTOR.CPP
//----------------------------------------------------------//
//-- Description			
// Establishes a TCP listener on a sevrer end, handles new
// incoming connection.
//----------------------------------------------------------//


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
// CTCPAcceptor::CTCPAcceptor
//----------------------------------------------------------//
//-- Description			
//----------------------------------------------------------//
CTCPAcceptor::CTCPAcceptor()
: m_eState(State::NotListening)
, m_pAddrResults(NULL)
, m_pCur(NULL)
, m_nSocket(SysSocket::INVALID_SOCK)
{
}


//----------------------------------------------------------//
// CTCPAcceptor::~CTCPAcceptor
//----------------------------------------------------------//
//-- Description			
//----------------------------------------------------------//
CTCPAcceptor::~CTCPAcceptor()
{
	StopListening();
}


//----------------------------------------------------------//
// CTCPAcceptor::CleanAddrResults
//----------------------------------------------------------//
//-- Description			
//----------------------------------------------------------//
void CTCPAcceptor::CleanAddrResults(void)
{
	if (IS_PTR(m_pAddrResults))
	{
		SysSocket::FreeInfo(m_pAddrResults);
		m_pAddrResults = NULL;
		m_pCur = NULL;
	}
}


//----------------------------------------------------------//
// CTCPAcceptor::CleanSocket
//----------------------------------------------------------//
//-- Description			
//----------------------------------------------------------//
void CTCPAcceptor::CleanSocket(void)
{
	if (SysSocket::INVALID_SOCK != m_nSocket)
	{
		SysSocket::CloseSocket(m_nSocket);
		m_nSocket = SysSocket::INVALID_SOCK;
	}
}


//----------------------------------------------------------//
// CTCPAcceptor::StopListening
//----------------------------------------------------------//
//-- Description			
//----------------------------------------------------------//
CTCPAcceptor::Error::Enum CTCPAcceptor::StopListening(void)
{
	CleanAddrResults();
	CleanSocket();

	m_eState = State::NotListening;

	return Error::Ok;
}


//----------------------------------------------------------//
// EXTERNALS
//----------------------------------------------------------//

//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//
