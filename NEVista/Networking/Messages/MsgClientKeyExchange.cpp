//----------------------------------------------------------//
// MSGCLIENTKEYEXCHANGE.CPP
//----------------------------------------------------------//
//-- Description			
// A key exchange message.
// Message containing the client half of an encryption key.
//----------------------------------------------------------//


#include "MsgClientKeyExchange.h"
#include "Types.h"
#include "Message.h"
#include "PacketSerializer.h"


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
// CMsgClientKeyExchange::CMsgClientKeyExchange
//----------------------------------------------------------//
//--Description
//----------------------------------------------------------//
CMsgClientKeyExchange::CMsgClientKeyExchange()
: CMessage(Type::MsgClientKeyExchange)
, m_nClientKey(0)
{
}


//----------------------------------------------------------//
// CMsgClientKeyExchange::~CMsgClientKeyExchange
//----------------------------------------------------------//
//--Description
//----------------------------------------------------------//
CMsgClientKeyExchange::~CMsgClientKeyExchange()
{
}


//----------------------------------------------------------//
// CMsgClientKeyExchange::Serialize
//----------------------------------------------------------//
//--Description
//----------------------------------------------------------//
size_t CMsgClientKeyExchange::Serialize(CSerializer& serializer)
{
	size_t nSize = CMessage::Serialize(serializer);
	nSize += serializer.SerializeU16(m_nClientKey, 'ckey');
	return nSize;
}


//----------------------------------------------------------//
// CMsgClientKeyExchange::SetKey
//----------------------------------------------------------//
//--Description
//----------------------------------------------------------//
void CMsgClientKeyExchange::SetKey(u16 nClientKey)
{
	m_nClientKey = nClientKey;
}


//----------------------------------------------------------//
// CMsgClientKeyExchange::GetKey
//----------------------------------------------------------//
//--Description
//----------------------------------------------------------//
u16 CMsgClientKeyExchange::GetKey(void) const
{
	return m_nClientKey;
}


//----------------------------------------------------------//
// EXTERNALS
//----------------------------------------------------------//

//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//
