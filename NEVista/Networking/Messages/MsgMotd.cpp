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
// IMPLEMENT_MESSAGE_REGISTRAR
//----------------------------------------------------------//
//--Description
//----------------------------------------------------------//
IMPLEMENT_MESSAGE_REGISTRAR(CMsgMotd)
{
	return new CMsgMotd();
}


//----------------------------------------------------------//
// CMsgMotd::CMsgMotd
//----------------------------------------------------------//
//--Description
//----------------------------------------------------------//
CMsgMotd::CMsgMotd()
: CMessage(kType, Flag::NeverEncrypted | Flag::ForcedEnd)
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

	nSize += serializer.SerializeU16(m_nMotdLength, 'len ');

	if (m_nMotdLength > 0) 
	{
		assert(m_nMotdLength < m_strMotd.Size());

		u8* pBuffer = (u8*)m_strMotd.Buffer();

		nSize += serializer.SerializeBytes(pBuffer, m_nMotdLength, 'motd');
		
		if (CSerializer::Mode::Deserializing == serializer.GetMode())
		{
			//-- If deserializing, make sure the string is null terminated.
			pBuffer[m_nMotdLength] = 0;
		}
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
	m_nMotdLength = m_strMotd.Length();
}


//----------------------------------------------------------//
// EXTERNALS
//----------------------------------------------------------//

//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//
