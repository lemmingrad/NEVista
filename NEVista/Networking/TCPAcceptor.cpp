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



CTCPAcceptor::CTCPAcceptor()
{
}

CTCPAcceptor::~CTCPAcceptor()
{
}

CTCPAcceptor::Result CTCPAcceptor::Listen(void)
{
	return Result();
}

CTCPAcceptor::Result CTCPAcceptor::Accept(void)
{
	return Result();
}