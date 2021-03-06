#ifndef _TCPBLOCKINGCONNECTOR_H_
#define _TCPBLOCKINGCONNECTOR_H_
#pragma once

//----------------------------------------------------------//
// TCPBLOCKINGCONNECTOR.H
//----------------------------------------------------------//
//-- Description			
// Establishes a TCP connection from a client end.
// BLOCKING.
//----------------------------------------------------------//


#include "Types.h"
#include "TCPConnector.h"
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


class CTCPBlockingConnector : public CTCPConnector
{
	public:

		CTCPBlockingConnector();
		~CTCPBlockingConnector();

		Result					Connect(const s8* strHostname, const s8* strPort);
};


//----------------------------------------------------------//
// EXTERNALS
//----------------------------------------------------------//

//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//

#endif //_TCPBLOCKINGCONNECTOR_H_