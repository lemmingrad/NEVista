#ifndef _TCPNONBLOCKINGCONNECTOR_H_
#define _TCPNONBLOCKINGCONNECTOR_H_
#pragma once

//----------------------------------------------------------//
// TCPNONBLOCKINGCONNECTOR.H
//----------------------------------------------------------//
//-- Description			
// Establishes a TCP connection from a client end.
// Asynchronous Non-blocking connector.
//----------------------------------------------------------//


#include "Types.h"
#include "TCPConnector.h"
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


class CTCPNonblockingConnector : public CTCPConnector
{
	public:

		CTCPNonblockingConnector();
		~CTCPNonblockingConnector();

		Error::Enum				Connect(const s8* strHostname, const s8* strPort);
		Result					Update(void);

	private:

		SysSocket::PollFd		m_pollFd;
};


//----------------------------------------------------------//
// EXTERNALS
//----------------------------------------------------------//

//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//

#endif //_TCPNONBLOCKINGCONNECTOR_H_