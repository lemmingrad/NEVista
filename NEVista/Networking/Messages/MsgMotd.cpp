


#include "MsgMotd.h"
#include "Types.h"
#include "SysString.h"
#include "SysMemory.h"
#include "Serialized.h"
#include "PacketSerializer.h"




CMsgMotd::CMsgMotd()
: CSerialized()
, m_nMotdLength(0)
{
	m_strMotd.Clear();
}

CMsgMotd::~CMsgMotd()
{
}

void CMsgMotd::Serialize(CPacketSerializer& Serializer)
{
	if (CSerializer::Mode::Serializing == Serializer.GetMode())
	{
		//-- Writing to packet stream
		m_nMotdLength = m_strMotd.Length();
		Serializer.SerializeU16(m_nMotdLength, 'len ');
		Serializer.SerializeBytes((u8*)m_strMotd.Buffer(), m_nMotdLength, 'motd');
	}
	else
	{
		//-- Reading from packet stream
		Serializer.SerializeU16(m_nMotdLength, 'len ');
		SysMemory::Memclear(m_strMotd.Buffer(), m_strMotd.Size());
		Serializer.SerializeBytes((u8*)m_strMotd.Buffer(), m_nMotdLength, 'motd');
	}
}
