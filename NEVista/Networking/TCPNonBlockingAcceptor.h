#ifndef _TCPNONBLOCKINGACCEPTOR_H_
#define _TCPNONBLOCKINGACCEPTOR_H_
#pragma once

//----------------------------------------------------------//
// TCPNONBLOCKINGACCEPTOR.H
//----------------------------------------------------------//
//-- Description			
// Establishes a TCP listener on a server end, handles new
// incoming connection.
// Asynchronous non-blocking acceptor.
//----------------------------------------------------------//


#include "Types.h"
#include "TCPAcceptor.h"
#include "Network.h"
#include "SysSocket.h"


//----------------------------------------------------------//
// DEFINES
//----------------------------------------------------------//

//----------------------------------------------------------//
// ENUMS
//----------------------------------------------------------//

//----------------------------------------------------------//
// FORWARD REFERENCES
//----------------------------------------------------------//

//----------------------------------------------------------//
// STRUCTS
//----------------------------------------------------------//

//----------------------------------------------------------//
// CLASSES
//----------------------------------------------------------//


class CTCPNonblockingAcceptor : public CTCPAcceptor
{
	public:

		CTCPNonblockingAcceptor();
		~CTCPNonblockingAcceptor();

		Error::Enum					Listen(const s8* strPort, s32 nBacklog = TCP_ACCEPTOR_BACKLOG);
		Result						Accept(void);

	private:

		SysSocket::PollFd			m_pollFd;
		s32							m_nBacklog;
};


//----------------------------------------------------------//
// EXTERNALS
//----------------------------------------------------------//

//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//

#endif //_TCPNONBLOCKINGACCEPTOR_H_