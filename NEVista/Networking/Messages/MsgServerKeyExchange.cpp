


#include "MsgServerKeyExchange.h"
#include "Types.h"
#include "SysString.h"
#include "SysMemory.h"
#include "PacketSerializer.h"




CMsgServerKeyExchange::CMsgServerKeyExchange()
: CMessage(Type::MsgServerKeyExchange)
, m_nServerKey(0)
{
}

CMsgServerKeyExchange::~CMsgServerKeyExchange()
{
}

size_t CMsgServerKeyExchange::Serialize(CSerializer& serializer)
{
	size_t nSize = 0;
	nSize += serializer.SerializeU16(m_nServerKey, 'skey');
	return nSize;
}

void CMsgServerKeyExchange::SetKey(u16 nServerKey)
{
	m_nServerKey = nServerKey;
}

u16 CMsgServerKeyExchange::GetKey(void)
{
	return m_nServerKey;
}
