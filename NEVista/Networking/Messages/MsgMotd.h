#ifndef _MSG_MOTD_H_
#define _MSG_MOTD_H_
#pragma once

//----------------------------------------------------------//
// MSGMOTD.H
//----------------------------------------------------------//
//-- Description			
// A message-of-the-day message.
// Message containing text. The first message sent by the 
// server to any connecting client.
//----------------------------------------------------------//


#include "Types.h"
#include "FixedString.h"
#include "Message.h"


//----------------------------------------------------------//
// DEFINES
//----------------------------------------------------------//


#define MSG_MOTD_MAX_SIZE (980)


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


class CMsgMotd : public CMessage
{
		DECLARE_MESSAGE_REGISTRAR(CMsgMotd, 'motd');

	public:

		CMsgMotd();
		virtual ~CMsgMotd();

		//-- CMessage
		virtual size_t						Serialize(ISerializer& serializer);

		const s8*							GetText(void) const;
		void								SetText(const s8* strBuffer);

	private:

		FixedString<MSG_MOTD_MAX_SIZE>		m_strMotd;
		u16									m_nMotdLength;
};


//----------------------------------------------------------//
// EXTERNALS
//----------------------------------------------------------//

//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//

#endif //_MSG_MOTD_H_