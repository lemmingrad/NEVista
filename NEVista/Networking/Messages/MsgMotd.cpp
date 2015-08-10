//----------------------------------------------------------//
// MSGMOTD.CPP
//----------------------------------------------------------//
//-- Description			
// A message-of-the-day message.
// Message containing text. The first message sent by the 
// server to any connecting client.
//----------------------------------------------------------//


#include "MsgMotd.h"
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
// CMsgMotd::CMsgMotd
//----------------------------------------------------------//
//--Description
//----------------------------------------------------------//
CMsgMotd::CMsgMotd()
: CMessage(Type::MsgMotd)
, m_nMotdLength(0)
{
	m_strMotd.Clear();
}


//----------------------------------------------------------//
// CMsgMotd::~CMsgMotd
//----------------------------------------------------------//
//--Description
//----------------------------------------------------------//
CMsgMotd::~CMsgMotd()
{
}


//----------------------------------------------------------//
// CMsgMotd::Serialize
//----------------------------------------------------------//
//--Description
//----------------------------------------------------------//
size_t CMsgMotd::Serialize(CSerializer& serializer)
{
	size_t nSize = CMessage::Serialize(serializer);

	if (CSerializer::Mode::Serializing == serializer.GetMode())
	{
		//-- Writing to packet stream
		m_nMotdLength = m_strMotd.Length();
	}
	else
	{
		//-- Reading from packet stream
		m_strMotd.Clear();
	}

	nSize += serializer.SerializeU16(m_nMotdLength, 'len ');
	if (m_nMotdLength > 0)
	{
		nSize += serializer.SerializeBytes((u8*)m_strMotd.Buffer(), m_nMotdLength, 'motd');
	}

	return nSize;
}


//----------------------------------------------------------//
// CMsgMotd::GetText
//----------------------------------------------------------//
//--Description
//----------------------------------------------------------//
const s8* CMsgMotd::GetText(void) const
{
	return m_strMotd.ConstBuffer();
}


//----------------------------------------------------------//
// CMsgMotd::SetText
//----------------------------------------------------------//
//--Description
//----------------------------------------------------------//
void CMsgMotd::SetText(const s8* strBuffer)
{
	m_strMotd.Set(strBuffer);
}


//----------------------------------------------------------//
// EXTERNALS
//----------------------------------------------------------//

//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//
