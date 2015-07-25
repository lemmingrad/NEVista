


#include "MsgMotd.h"
#include "Types.h"
#include "SysString.h"
#include "SysMemory.h"
#include "Serialized.h"
#include "PacketSerializer.h"




CMsgMotd::CMsgMotd()
: CMessage(Type::MsgMotd)
, m_nMotdLength(0)
{
	m_strMotd.Clear();
}

CMsgMotd::~CMsgMotd()
{
}

size_t CMsgMotd::Serialize(CSerializer& serializer)
{
	size_t nSize = 0;

	if (CSerializer::Mode::Serializing == serializer.GetMode())
	{
		//-- Writing to packet stream
		u32 eType = (u32)m_eType;
		nSize += serializer.SerializeU32(eType, 'type');
		m_nMotdLength = m_strMotd.Length();
		nSize += serializer.SerializeU16(m_nMotdLength, 'len ');
		nSize += serializer.SerializeBytes((u8*)m_strMotd.Buffer(), m_nMotdLength, 'motd');
	}
	else
	{
		//-- Reading from packet stream
		nSize += serializer.SerializeU16(m_nMotdLength, 'len ');
		SysMemory::Memclear(m_strMotd.Buffer(), m_strMotd.Size());
		nSize += serializer.SerializeBytes((u8*)m_strMotd.Buffer(), m_nMotdLength, 'motd');
	}

	return nSize;
}
