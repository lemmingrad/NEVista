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
// IMPLEMENT_MESSAGE_REGISTRAR
//----------------------------------------------------------//
//--Description
//----------------------------------------------------------//
IMPLEMENT_MESSAGE_REGISTRAR(CMsgServerKeyExchange)
{
	return new CMsgServerKeyExchange();
}


//----------------------------------------------------------//
// CMsgServerKeyExchange::CMsgServerKeyExchange
//----------------------------------------------------------//
//--Description
//----------------------------------------------------------//
CMsgServerKeyExchange::CMsgServerKeyExchange()
: CMessage(kType, Flag::NeverEncrypted | Flag::ForcedEnd)
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
	nSize += serializer.SerializeU8(m_nServerKey, 'skey');
	return nSize;
}


//----------------------------------------------------------//
// CMsgServerKeyExchange::SetKey
//----------------------------------------------------------//
//--Description
//----------------------------------------------------------//
void CMsgServerKeyExchange::SetKey(u8 nServerKey)
{
	m_nServerKey = nServerKey;
}


//----------------------------------------------------------//
// CMsgServerKeyExchange::GetKey
//----------------------------------------------------------//
//--Description
//----------------------------------------------------------//
u8 CMsgServerKeyExchange::GetKey(void) const
{
	return m_nServerKey;
}


//----------------------------------------------------------//
// EXTERNALS
//----------------------------------------------------------//

//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//

