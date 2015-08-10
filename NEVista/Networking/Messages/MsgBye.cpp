//----------------------------------------------------------//
// MSGBYE.CPP
//----------------------------------------------------------//
//-- Description			
// A disconnect message.
// Message indicating a disconnect, with optional text.
//----------------------------------------------------------//


#include "MsgBye.h"
#include "Types.h"
#include "FixedString.h"
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
// CMsgBye::CMsgBye
//----------------------------------------------------------//
//--Description
//----------------------------------------------------------//
CMsgBye::CMsgBye(Reason::Enum eReason)
: CMessage(Type::MsgMotd)
, m_eReason(eReason)
, m_nStrLength(0)
{
	m_strReason.Clear();
}

//----------------------------------------------------------//
// CMsgBye::CMsgBye
//----------------------------------------------------------//
//--Description
//----------------------------------------------------------//
CMsgBye::CMsgBye()
: CMessage(Type::MsgMotd)
, m_eReason(Reason::Unknown)
, m_nStrLength(0)
{
	m_strReason.Clear();
}


//----------------------------------------------------------//
// CMsgBye::~CMsgBye
//----------------------------------------------------------//
//--Description
//----------------------------------------------------------//
CMsgBye::~CMsgBye()
{
}


//----------------------------------------------------------//
// CMsgBye::Serialize
//----------------------------------------------------------//
//--Description
//----------------------------------------------------------//
size_t CMsgBye::Serialize(CSerializer& serializer)
{
	size_t nSize = CMessage::Serialize(serializer);

	nSize += serializer.SerializeU32((u32&)m_eReason, 'ersn');

	if (CSerializer::Mode::Serializing == serializer.GetMode())
	{
		//-- Writing to packet stream
		m_nStrLength = m_strReason.Length();
	}
	else
	{
		//-- Reading from packet stream
		m_strReason.Clear();
	}

	nSize += serializer.SerializeU16(m_nStrLength, 'len ');
	if (m_nStrLength > 0)
	{
		nSize += serializer.SerializeBytes((u8*)m_strReason.Buffer(), m_nStrLength, 'srsn');
	}

	return nSize;
}


//----------------------------------------------------------//
// CMsgBye::GetReason
//----------------------------------------------------------//
//--Description
//----------------------------------------------------------//
CMsgBye::Reason::Enum CMsgBye::GetReason(void) const
{
	return m_eReason;
}


//----------------------------------------------------------//
// CMsgBye::SetReason
//----------------------------------------------------------//
//--Description
//----------------------------------------------------------//
void CMsgBye::SetReason(CMsgBye::Reason::Enum eReason)
{
	m_eReason = eReason;
}


//----------------------------------------------------------//
// CMsgBye::GetText
//----------------------------------------------------------//
//--Description
//----------------------------------------------------------//
const s8* CMsgBye::GetText(void) const
{
	return m_strReason.ConstBuffer();
}


//----------------------------------------------------------//
// CMsgBye::SetText
//----------------------------------------------------------//
//--Description
//----------------------------------------------------------//
void CMsgBye::SetText(const s8* strBuffer)
{
	m_strReason.Set(strBuffer);
}


//----------------------------------------------------------//
// EXTERNALS
//----------------------------------------------------------//

//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//
