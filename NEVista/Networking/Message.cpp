//----------------------------------------------------------//
// MESSAGE.CPP
//----------------------------------------------------------//
//-- Description			
// A message class. 
// Groups of messages are sent across the connection in a 
// packet. Each packet may or may not be encrypted and/or
// compressed. Packets are guaranteed to contain at least
// one message, and all messages must be complete.
//----------------------------------------------------------//


#include "Message.h"
#include "Types.h"
#include "Messages/MsgMotd.h"


//----------------------------------------------------------//
// DEFINES
//----------------------------------------------------------//

//----------------------------------------------------------//
// ENUMS
//----------------------------------------------------------//

//----------------------------------------------------------//
// STRUCTS
//----------------------------------------------------------//


//----------------------------------------------------------//
// CMessage::CreateType
//----------------------------------------------------------//
//--Description
// Static factory function to create a message of a given type.
//----------------------------------------------------------//
CMessage* CMessage::CreateType(CMessage::Type::Enum eType)
{
	switch (eType)
	{
		case 'motd':
		{
			return new CMsgMotd();
		}
		break;

		default:
		break;
	}

	return NULL;
}


CMessage::CMessage(CMessage::Type::Enum eType)
: m_eType(eType)
{
}

CMessage::~CMessage()
{
}


//----------------------------------------------------------//
// EXTERNALS
//----------------------------------------------------------//

//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//
