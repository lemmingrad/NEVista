#ifndef _MSG_BYE_H_
#define _MSG_BYE_H_
#pragma once

//----------------------------------------------------------//
// MSGBYE.H
//----------------------------------------------------------//
//-- Description			
// A disconnect message.
// Message indicating a disconnect, with optional text.
//----------------------------------------------------------//


#include "Types.h"
#include "FixedString.h"
#include "Message.h"


//----------------------------------------------------------//
// DEFINES
//----------------------------------------------------------//


#define MSG_BYE_MAX_SIZE (980)


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


class CMsgBye : public CMessage
{
		DECLARE_MESSAGE_REGISTRAR('bye ', CMsgBye);

	public:

		struct Reason
		{
			enum Enum 
			{
				Unknown = 0,
				SafeDisconnect,
				SafeDisconnectACK
			};
		};

		CMsgBye(Reason::Enum eReason);
		CMsgBye();
		virtual ~CMsgBye();

		//-- CMessage
		virtual size_t						Serialize(CSerializer& serializer);

		Reason::Enum						GetReason(void) const;
		void								SetReason(Reason::Enum eReason);

		const s8*							GetText(void) const;
		void								SetText(const s8* strBuffer);

	private:

		Reason::Enum						m_eReason;
		FixedString<MSG_BYE_MAX_SIZE>		m_strReason;
		u16									m_nStrLength;
};


//----------------------------------------------------------//
// EXTERNALS
//----------------------------------------------------------//

//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//

#endif //_MSG_BYE_H_