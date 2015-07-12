


#include "MsgClientKeyExchange.h"
#include "Types.h"
#include "SysString.h"
#include "SysMemory.h"
#include "PacketSerializer.h"




CMsgClientKeyExchange::CMsgClientKeyExchange()
: m_nClientKey(0)
{
}

CMsgClientKeyExchange::~CMsgClientKeyExchange()
{
}

void CMsgClientKeyExchange::Serialize(CPacketSerializer& Serializer)
{
	Serializer.SerializeU16(m_nClientKey, 'ckey');
}

void CMsgClientKeyExchange::SetKey(u16 nClientKey)
{
	m_nClientKey = nClientKey;
}

u16 CMsgClientKeyExchange::GetKey(void)
{
	return m_nClientKey;
}
