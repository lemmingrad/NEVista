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
				MsgBye											= 'bye ',
				MsgMotd											= 'motd',
				MsgLogin										= 'logn',
				MsgClientKeyExchange							= 'ckey',
				MsgServerKeyExchange							= 'skey',
				MsgChat											= 'chat'
				//-- Add new message IDs here.
			};
		};

		CMessage(Type::Enum eType);
		virtual ~CMessage();

		static CMessage*		CreateType(Type::Enum eType);
		
		Type::Enum				GetType(void);

		//-- CSerialized
		virtual size_t			Serialize(CSerializer& Serializer);

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