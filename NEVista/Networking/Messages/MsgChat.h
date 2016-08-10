#ifndef _MSG_CHAT_H_
#define _MSG_CHAT_H_
#pragma once

//----------------------------------------------------------//
// MSGCHAT.H
//----------------------------------------------------------//
//-- Description			
// A text chat message.
// Includes shouts, talk and whispers.
//----------------------------------------------------------//


#include "Types.h"
#include "FixedString.h"
#include "Message.h"


//----------------------------------------------------------//
// DEFINES
//----------------------------------------------------------//


#define MSG_CHAT_MAX_SIZE (980)


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


class CMsgChat : public CMessage
{
		DECLARE_MESSAGE_REGISTRAR(CMsgChat, 'chat');

	public:

		struct Type
		{
			enum Enum 
			{
				Chat = 0,
				Shout,
				Whisper,
				Server
			};
		};

		CMsgChat(Type::Enum eType);
		CMsgChat();
		virtual ~CMsgChat();

		//-- CMessage
		virtual size_t						Serialize(CSerializer& serializer);

		Type::Enum							GetType(void) const;
		void								SetType(Type::Enum eType);

		const s8*							GetText(void) const;
		void								SetText(const s8* strBuffer);

	private:

		Type::Enum							m_eType;
		FixedString<MSG_CHAT_MAX_SIZE>		m_strText;
		u16									m_nStrLength;
};


//----------------------------------------------------------//
// EXTERNALS
//----------------------------------------------------------//

//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//

#endif //_MSG_CHAT_H_