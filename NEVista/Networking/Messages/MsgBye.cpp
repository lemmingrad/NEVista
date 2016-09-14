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
// GLOBALS
//----------------------------------------------------------//


CMsgBye::Reason CMsgBye::sm_ReasonTable[CMsgBye::Reason::MAX] = {
	{ "Unknown reason" },
	{ "Safe disconnect" },
	{ "Client disconnected" },
	{ "Server disconnected" },
	{ "Bye acknowledged" },
	{ "Unexpected disconnection" },
	{ "Timeout" },
	{ "Invalid packet header" },
	{ "Protocol error" },
	{ "No encryption key was negotiated" },
	{ "Login failed - Invalid version" },
	{ "Login failed - No such user" },
	{ "Login failed - Incorrect password" },
	{ "Server is going down NOW!" }
};


//----------------------------------------------------------//
// IMPLEMENT_MESSAGE_REGISTRAR
//----------------------------------------------------------//
//--Description
//----------------------------------------------------------//
IMPLEMENT_MESSAGE_REGISTRAR(CMsgBye, "ByeBye")
{
	return new CMsgBye(Reason::Unknown);
}


//----------------------------------------------------------//
// CMsgBye::CMsgBye
//----------------------------------------------------------//
//--Description
//----------------------------------------------------------//
CMsgBye::CMsgBye(Reason::Enum eReason)
: CMessage(kType, Flag::ForcedEnd)
, m_eReason(eReason)
, m_nStrLength(0)
{
	m_strReason.Clear();
	SetReason(eReason);
}


//----------------------------------------------------------//
// CMsgBye::CMsgBye
//----------------------------------------------------------//
//--Description
//----------------------------------------------------------//
CMsgBye::CMsgBye()
: CMessage(kType, Flag::ForcedEnd)
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
size_t CMsgBye::Serialize(ISerializer& serializer)
{
	size_t nSize = CMessage::Serialize(serializer);

	nSize += serializer.SerializeU32((u32&)m_eReason, 'ersn');
	nSize += serializer.SerializeU16(m_nStrLength, 'len ');

	if (m_nStrLength > 0) 
	{
		assert(m_nStrLength < m_strReason.Size());

		u8* pBuffer = (u8*)m_strReason.Buffer();

		nSize += serializer.SerializeBytes(pBuffer, m_nStrLength, 'srsn');
		
		if (ISerializer::Mode::Deserializing == serializer.GetMode())
		{
			//-- If deserializing, make sure the string is null terminated.
			pBuffer[m_nStrLength] = 0;
		}
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
	SetText(sm_ReasonTable[eReason].m_strReasonString);
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
	if (IS_PTR(m_strReason.Set(strBuffer)))
	{
		m_nStrLength = m_strReason.Length();
	}
}


//----------------------------------------------------------//
// EXTERNALS
//----------------------------------------------------------//

//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//
