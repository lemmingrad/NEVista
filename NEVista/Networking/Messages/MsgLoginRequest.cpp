//----------------------------------------------------------//
// MSGLOGINREQUEST.CPP
//----------------------------------------------------------//
//-- Description			
// A login request message sent by the client.
//----------------------------------------------------------//


#include "MsgLoginRequest.h"
#include "Types.h"
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
IMPLEMENT_MESSAGE_REGISTRAR(CMsgLoginRequest, "LoginRequest")
{
	return new CMsgLoginRequest();
}


//----------------------------------------------------------//
// CMsgLoginRequest::CMsgLoginRequest
//----------------------------------------------------------//
//--Description
//----------------------------------------------------------//
CMsgLoginRequest::CMsgLoginRequest()
: CMessage(kType, Flag::AlwaysEncrypted)
, m_nStrNameLength(0)
, m_nStrPasswordLength(0)
{
	m_strName.Clear();
	m_strPassword.Clear();
}


CMsgLoginRequest::CMsgLoginRequest(const s8* strName, const s8* strPassword)
: CMessage(kType, Flag::AlwaysEncrypted)
, m_nStrNameLength(0)
, m_nStrPasswordLength(0)
{
	SetName(strName);
	SetPassword(strPassword);
}


//----------------------------------------------------------//
// CMsgLoginRequest::~CMsgLoginRequest
//----------------------------------------------------------//
//--Description
//----------------------------------------------------------//
CMsgLoginRequest::~CMsgLoginRequest()
{
}


//----------------------------------------------------------//
// CMsgLoginRequest::Serialize
//----------------------------------------------------------//
//--Description
//----------------------------------------------------------//
size_t CMsgLoginRequest::Serialize(ISerializer& serializer)
{
	size_t nSize = CMessage::Serialize(serializer);

	nSize += serializer.SerializeU8(m_nStrNameLength, 'lenN');
	nSize += serializer.SerializeU8(m_nStrPasswordLength, 'lenP');

	if ( (m_nStrNameLength > 0) 
		&& (m_nStrPasswordLength > 0) )
	{
		assert(m_nStrNameLength < m_strName.Size());
		assert(m_nStrPasswordLength < m_strPassword.Size());

		u8* pNameBuffer = (u8*)m_strName.Buffer();
		u8* pPasswordBuffer = (u8*)m_strPassword.Buffer();

		nSize += serializer.SerializeBytes(pNameBuffer, m_nStrNameLength, 'name');
		nSize += serializer.SerializeBytes(pPasswordBuffer, m_nStrPasswordLength, 'pass');
	
		if (ISerializer::Mode::Deserializing == serializer.GetMode())
		{
			//-- If deserializing, make sure the string is null terminated.
			pNameBuffer[m_nStrNameLength] = 0;
			pPasswordBuffer[m_nStrPasswordLength] = 0;
		}
	}
	
	return nSize;
}


//----------------------------------------------------------//
// CMsgLoginRequest::GetName
//----------------------------------------------------------//
//--Description
//----------------------------------------------------------//
const s8* CMsgLoginRequest::GetName(void) const
{
	return m_strName.ConstBuffer();
}


//----------------------------------------------------------//
// CMsgLoginRequest::SetName
//----------------------------------------------------------//
//--Description
//----------------------------------------------------------//
void CMsgLoginRequest::SetName(const s8* strBuffer)
{
	m_strName.Set(strBuffer);
	m_nStrNameLength = m_strName.Length();
}


//----------------------------------------------------------//
// CMsgLoginRequest::GetPassword
//----------------------------------------------------------//
//--Description
//----------------------------------------------------------//
const s8* CMsgLoginRequest::GetPassword(void) const
{
	return m_strPassword.ConstBuffer();
}


//----------------------------------------------------------//
// CMsgLoginRequest::SetPassword
//----------------------------------------------------------//
//--Description
//----------------------------------------------------------//
void CMsgLoginRequest::SetPassword(const s8* strBuffer)
{
	m_strPassword.Set(strBuffer);
	m_nStrPasswordLength = m_strPassword.Length();
}


//----------------------------------------------------------//
// EXTERNALS
//----------------------------------------------------------//

//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//
