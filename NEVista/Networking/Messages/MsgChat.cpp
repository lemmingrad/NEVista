//----------------------------------------------------------//
// MSGCHAT.CPP
//----------------------------------------------------------//
//-- Description			
// A text chat message.
// Includes shouts, talk and whispers.
//----------------------------------------------------------//


#include "MsgChat.h"
#include "Types.h"
#include "FixedString.h"
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
IMPLEMENT_MESSAGE_REGISTRAR(CMsgChat, "Chat")
{
	return new CMsgChat();
}


//----------------------------------------------------------//
// CMsgChat::CMsgChat
//----------------------------------------------------------//
//--Description
//----------------------------------------------------------//
CMsgChat::CMsgChat(Type::Enum eType)
: CMessage(kType, 0)
, m_eType(eType)
, m_nStrLength(0)
{
	m_strText.Clear();
}


//----------------------------------------------------------//
// CMsgChat::CMsgChat
//----------------------------------------------------------//
//--Description
//----------------------------------------------------------//
CMsgChat::CMsgChat()
: CMessage(kType, 0)
, m_eType(Type::Chat)
, m_nStrLength(0)
{
	m_strText.Clear();
}


//----------------------------------------------------------//
// CMsgChat::~CMsgChat
//----------------------------------------------------------//
//--Description
//----------------------------------------------------------//
CMsgChat::~CMsgChat()
{
}


//----------------------------------------------------------//
// CMsgChat::Serialize
//----------------------------------------------------------//
//--Description
//----------------------------------------------------------//
size_t CMsgChat::Serialize(ISerializer& serializer)
{
	size_t nSize = CMessage::Serialize(serializer);

	nSize += serializer.SerializeU32((u32&)m_eType, 'type');
	nSize += serializer.SerializeU16(m_nStrLength, 'len ');

	if (m_nStrLength > 0) 
	{
		assert(m_nStrLength < m_strText.Size());

		u8* pBuffer = (u8*)m_strText.Buffer();

		nSize += serializer.SerializeBytes(pBuffer, m_nStrLength, 'text');
		
		if (ISerializer::Mode::Deserializing == serializer.GetMode())
		{
			//-- If deserializing, make sure the string is null terminated.
			pBuffer[m_nStrLength] = 0;
		}
	}

	return nSize;
}


//----------------------------------------------------------//
// CMsgChat::GetType
//----------------------------------------------------------//
//--Description
//----------------------------------------------------------//
CMsgChat::Type::Enum CMsgChat::GetType(void) const
{
	return m_eType;
}


//----------------------------------------------------------//
// CMsgChat::SetReason
//----------------------------------------------------------//
//--Description
//----------------------------------------------------------//
void CMsgChat::SetType(CMsgChat::Type::Enum eType)
{
	m_eType = eType;
}


//----------------------------------------------------------//
// CMsgChat::GetText
//----------------------------------------------------------//
//--Description
//----------------------------------------------------------//
const s8* CMsgChat::GetText(void) const
{
	return m_strText.ConstBuffer();
}


//----------------------------------------------------------//
// CMsgChat::SetText
//----------------------------------------------------------//
//--Description
//----------------------------------------------------------//
void CMsgChat::SetText(const s8* strBuffer)
{
	if (IS_PTR(m_strText.Set(strBuffer)))
	{
		m_nStrLength = m_strText.Length();
	}
}


//----------------------------------------------------------//
// EXTERNALS
//----------------------------------------------------------//

//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//
