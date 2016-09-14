//----------------------------------------------------------//
// MESSAGE.CPP
//----------------------------------------------------------//
//-- Description			
// A message class. 
// Groups of messages are sent across the connection in a 
// packet. Each packet may or may not be encrypted and/or
// compressed. Packets are guaranteed to contain at least
// one message, and all messages must be complete.
//----------------------------------------------------------//


#include "Message.h"
#include "Types.h"
#include "PacketSerializer.h"
#include "Messages/MsgBye.h"
#include "Messages/MsgMotd.h"
#include <map>


//----------------------------------------------------------//
// DEFINES
//----------------------------------------------------------//

//----------------------------------------------------------//
// GLOBALS
//----------------------------------------------------------//

//----------------------------------------------------------//
// CMessage::CMessage
//----------------------------------------------------------//
//--Description
//----------------------------------------------------------//
CMessage::CMessage(CMessage::Type nType, bitfield bitFlags)
: m_nType(nType)
, m_bitFlags(bitFlags)
{
}


//----------------------------------------------------------//
// CMessage::~CMessage
//----------------------------------------------------------//
//--Description
//----------------------------------------------------------//
CMessage::~CMessage()
{
}


//----------------------------------------------------------//
// CMessage::Serialize
//----------------------------------------------------------//
//--Description
//----------------------------------------------------------//
size_t CMessage::Serialize(ISerializer& serializer)
{
	size_t nSize = 0;

	if (ISerializer::Mode::Serializing == serializer.GetMode())
	{
		nSize += serializer.SerializeU32(m_nType, 'type');
	}
	else
	{
	}

	return nSize;
}


//----------------------------------------------------------//
// CMessage::GetType
//----------------------------------------------------------//
//--Description
//----------------------------------------------------------//
CMessage::Type CMessage::GetType(void) const
{
	return m_nType;
}


//----------------------------------------------------------//
// CMessage::CanBeEncrypted
//----------------------------------------------------------//
//--Description
//----------------------------------------------------------//
bool CMessage::CanBeEncrypted(bool bRecommended) const
{
	if (TEST_FLAG(m_bitFlags, Flag::NeverEncrypted))
	{
		return false;
	}
	else if (TEST_FLAG(m_bitFlags, Flag::AlwaysEncrypted))
	{
		return true;
	}

	return bRecommended;
}


//----------------------------------------------------------//
// CMessage::CanBeCompressed
//----------------------------------------------------------//
//--Description
//----------------------------------------------------------//
bool CMessage::CanBeCompressed(bool bRecommended) const
{
	if (TEST_FLAG(m_bitFlags, Flag::NeverCompressed))
	{
		return false;
	}
	else if (TEST_FLAG(m_bitFlags, Flag::AlwaysCompressed))
	{
		return true;
	}

	return bRecommended;
}


//----------------------------------------------------------//
// CMessage::IsForcedEnd
//----------------------------------------------------------//
//--Description
//----------------------------------------------------------//
bool CMessage::IsForcedEnd(void) const
{
	return TEST_FLAG(m_bitFlags, Flag::ForcedEnd);
}


//----------------------------------------------------------//
// CMessageFactory::CMessageFactory
//----------------------------------------------------------//
//--Description
// CMessage class factory
//----------------------------------------------------------//
CMessageFactory::CMessageFactory()
{
}


//----------------------------------------------------------//
// CMessageFactory::~CMessageFactory
//----------------------------------------------------------//
//--Description
//----------------------------------------------------------//
CMessageFactory::~CMessageFactory()
{
}


//----------------------------------------------------------//
// CMessageFactory::GetMap
//----------------------------------------------------------//
//--Description
//----------------------------------------------------------//
CMessageFactory::TFuncMap& CMessageFactory::GetMap(void)
{
	static CMessageFactory::TFuncMap s_Map;
	return s_Map;
}


//----------------------------------------------------------//
// CMessageFactory::RegisterType
//----------------------------------------------------------//
//--Description
//----------------------------------------------------------//
void CMessageFactory::RegisterType(CMessage::Type nType, CMessageFactory::TNameFunc nameFunc, CMessageFactory::TCreateFunc createFunc)
{
	FuncSet set;
	set.nameFunc = nameFunc;
	set.createFunc = createFunc;
	GetMap()[nType] = set;
}


//----------------------------------------------------------//
// CMessageFactory::GetTypeString
//----------------------------------------------------------//
//--Description
// Return a string representation for the given type.
//----------------------------------------------------------//
const s8* CMessageFactory::GetTypeString(CMessage::Type nType)
{
	TFuncMap& map = GetMap();
	TFuncMap::const_iterator cit = map.find(nType);
	if (cit != map.end())
	{
		return cit->second.nameFunc();
	}

	return "Unknown";
}


//----------------------------------------------------------//
// CMessageFactory::CreateType
//----------------------------------------------------------//
//--Description
// Create a message of a given type.
//----------------------------------------------------------//
CMessage* CMessageFactory::CreateType(CMessage::Type nType)
{
	TFuncMap& map = GetMap();
	TFuncMap::const_iterator cit = map.find(nType);
	if (cit != map.end())
	{
		return cit->second.createFunc();
	}

	return NULL;
}


//----------------------------------------------------------//
// EXTERNALS
//----------------------------------------------------------//

//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//
