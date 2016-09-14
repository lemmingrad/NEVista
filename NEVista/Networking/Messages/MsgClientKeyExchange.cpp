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
// GLOBALS
//----------------------------------------------------------//

//----------------------------------------------------------//
// IMPLEMENT_MESSAGE_REGISTRAR
//----------------------------------------------------------//
//--Description
//----------------------------------------------------------//
IMPLEMENT_MESSAGE_REGISTRAR(CMsgClientKeyExchange, "ClientKeyExchange")
{
	return new CMsgClientKeyExchange();
}


//----------------------------------------------------------//
// CMsgClientKeyExchange::CMsgClientKeyExchange
//----------------------------------------------------------//
//--Description
//----------------------------------------------------------//
CMsgClientKeyExchange::CMsgClientKeyExchange()
: CMessage(kType, Flag::NeverEncrypted | Flag::ForcedEnd)
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
size_t CMsgClientKeyExchange::Serialize(ISerializer& serializer)
{
	size_t nSize = CMessage::Serialize(serializer);
	nSize += serializer.SerializeU8(m_nClientKey, 'ckey');
	return nSize;
}


//----------------------------------------------------------//
// CMsgClientKeyExchange::SetKey
//----------------------------------------------------------//
//--Description
//----------------------------------------------------------//
void CMsgClientKeyExchange::SetKey(u8 nClientKey)
{
	m_nClientKey = nClientKey;
}


//----------------------------------------------------------//
// CMsgClientKeyExchange::GetKey
//----------------------------------------------------------//
//--Description
//----------------------------------------------------------//
u8 CMsgClientKeyExchange::GetKey(void) const
{
	return m_nClientKey;
}


//----------------------------------------------------------//
// EXTERNALS
//----------------------------------------------------------//

//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//
