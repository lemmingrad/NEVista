


#include "MsgServerKeyExchange.h"
#include "Types.h"
#include "SysString.h"
#include "SysMemory.h"
#include "PacketSerializer.h"




CMsgServerKeyExchange::CMsgServerKeyExchange()
: m_nServerKey(0)
{
}

CMsgServerKeyExchange::~CMsgServerKeyExchange()
{
}

void CMsgServerKeyExchange::Serialize(CPacketSerializer& Serializer)
{
	Serializer.SerializeU16(m_nServerKey, 'skey');
}

void CMsgServerKeyExchange::SetKey(u16 nServerKey)
{
	m_nServerKey = nServerKey;
}

u16 CMsgServerKeyExchange::GetKey(void)
{
	return m_nServerKey;
}
