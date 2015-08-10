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
#include "PacketSerializer.h"
#include "Messages/MsgBye.h"
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
		case Type::MsgBye:
		{
			return new CMsgBye(CMsgBye::Reason::SafeDisconnect);
		}
		break;

		case Type::MsgMotd:
		{
			return new CMsgMotd();
		}
		break;

		default:
		break;
	}

	return NULL;
}


//----------------------------------------------------------//
// CMessage::CMessage
//----------------------------------------------------------//
//--Description
//----------------------------------------------------------//
CMessage::CMessage(CMessage::Type::Enum eType)
: m_eType(eType)
{
}


//----------------------------------------------------------//
// CMessage::~CMessage
//----------------------------------------------------------//
//--Description
//----------------------------------------------------------//
CMessage::~CMessage()
{
}


//----------------------------------------------------------//
// CMessage::Serialize
//----------------------------------------------------------//
//--Description
//----------------------------------------------------------//
size_t CMessage::Serialize(CSerializer& serializer)
{
	size_t nSize = 0;

	if (CSerializer::Mode::Serializing == serializer.GetMode())
	{
		u32 eType = (u32)m_eType;
		nSize += serializer.SerializeU32(eType, 'type');
	}
	else
	{
	}

	return nSize;
}


//----------------------------------------------------------//
// CMessage::GetType
//----------------------------------------------------------//
//--Description
//----------------------------------------------------------//
CMessage::Type::Enum CMessage::GetType(void)
{
	return m_eType;
}


//----------------------------------------------------------//
// EXTERNALS
//----------------------------------------------------------//

//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//
