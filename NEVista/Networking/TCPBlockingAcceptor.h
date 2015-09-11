#ifndef _TCPBLOCKINGACCEPTOR_H_
#define _TCPBLOCKINGACCEPTOR_H_
#pragma once

//----------------------------------------------------------//
// TCPBLOCKINGACCEPTOR.H
//----------------------------------------------------------//
//-- Description			
// Establishes a TCP listener on a server end, handles new
// incoming connection. BLOCKING.
//----------------------------------------------------------//


#include "Types.h"
#include "TCPAcceptor.h"
#include "Network.h"


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


class CTCPBlockingAcceptor : public CTCPAcceptor
{
	public:

		CTCPBlockingAcceptor();
		~CTCPBlockingAcceptor();

		Error::Enum					Listen(const s8* strPort, s32 nBacklog = TCP_ACCEPTOR_BACKLOG);
		Result						Accept(void);
};


//----------------------------------------------------------//
// EXTERNALS
//----------------------------------------------------------//

//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//

#endif //_TCPBLOCKINGACCEPTOR_H_