


#include "MsgClientKeyExchange.h"
#include "Types.h"
#include "SysString.h"
#include "SysMemory.h"
#include "PacketSerializer.h"




CMsgClientKeyExchange::CMsgClientKeyExchange()
: CMessage(Type::MsgClientKeyExchange)
, m_nClientKey(0)
{
}

CMsgClientKeyExchange::~CMsgClientKeyExchange()
{
}

size_t CMsgClientKeyExchange::Serialize(CSerializer& serializer)
{
	size_t nSize = 0;
	nSize += serializer.SerializeU16(m_nClientKey, 'ckey');
	return nSize;
}

void CMsgClientKeyExchange::SetKey(u16 nClientKey)
{
	m_nClientKey = nClientKey;
}

u16 CMsgClientKeyExchange::GetKey(void)
{
	return m_nClientKey;
}
