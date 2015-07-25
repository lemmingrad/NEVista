#ifndef _MESSAGE_H_
#define _MESSAGE_H_
#pragma once

//----------------------------------------------------------//
// MESSAGE.H
//----------------------------------------------------------//
//-- Description			
// A message class. 
// Groups of messages are sent across the connection in a 
// packet. Each packet may or may not be encrypted and/or
// compressed. Packets are guaranteed to contain at least
// one message, and all messages must be complete.
//----------------------------------------------------------//


#include "Types.h"
#include "Serialized.h"


//----------------------------------------------------------//
// DEFINES
//----------------------------------------------------------//

//----------------------------------------------------------//
// ENUMS
//----------------------------------------------------------//

//----------------------------------------------------------//
// FORWARD REFERENCES
//----------------------------------------------------------//


class CSerializer;


//----------------------------------------------------------//
// STRUCTS
//----------------------------------------------------------//

//----------------------------------------------------------//
// CLASSES
//----------------------------------------------------------//


class CMessage : public CSerialized
{
	public:

		struct Type 
		{
			enum Enum
			{
				Unknown											= 'xxxx',
				MsgMotd											= 'motd',
				MsgClientKeyExchange							= 'ckey',
				MsgServerKeyExchange							= 'skey'
			};
		};

		CMessage(Type::Enum eType);
		virtual ~CMessage();

		static CMessage*		CreateType(Type::Enum eType);


		//-- CSerialized
		virtual size_t			Serialize(CSerializer& Serializer) = 0;

	protected:

		Type::Enum				m_eType;
};


//----------------------------------------------------------//
// EXTERNALS
//----------------------------------------------------------//

//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//

#endif //_MESSAGE_H_