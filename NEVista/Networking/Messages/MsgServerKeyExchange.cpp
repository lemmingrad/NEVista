//----------------------------------------------------------//
// MSGSERVERKEYEXCHANGE.CPP
//----------------------------------------------------------//
//-- Description			
// A key exchange message.
// Message containing the server half of an encryption key.
//----------------------------------------------------------//


#include "MsgServerKeyExchange.h"
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
// CMsgServerKeyExchange::CMsgServerKeyExchange
//----------------------------------------------------------//
//--Description
//----------------------------------------------------------//
CMsgServerKeyExchange::CMsgServerKeyExchange()
: CMessage(Type::MsgServerKeyExchange)
, m_nServerKey(0)
{
}


//----------------------------------------------------------//
// CMsgServerKeyExchange::~CMsgServerKeyExchange
//----------------------------------------------------------//
//--Description
//----------------------------------------------------------//
CMsgServerKeyExchange::~CMsgServerKeyExchange()
{
}


//----------------------------------------------------------//
// CMsgServerKeyExchange::Serialize
//----------------------------------------------------------//
//--Description
//----------------------------------------------------------//
size_t CMsgServerKeyExchange::Serialize(CSerializer& serializer)
{
	size_t nSize = CMessage::Serialize(serializer);
	nSize += serializer.SerializeU16(m_nServerKey, 'skey');
	return nSize;
}


//----------------------------------------------------------//
// CMsgServerKeyExchange::SetKey
//----------------------------------------------------------//
//--Description
//----------------------------------------------------------//
void CMsgServerKeyExchange::SetKey(u16 nServerKey)
{
	m_nServerKey = nServerKey;
}


//----------------------------------------------------------//
// CMsgServerKeyExchange::GetKey
//----------------------------------------------------------//
//--Description
//----------------------------------------------------------//
u16 CMsgServerKeyExchange::GetKey(void) const
{
	return m_nServerKey;
}


//----------------------------------------------------------//
// EXTERNALS
//----------------------------------------------------------//

//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//

