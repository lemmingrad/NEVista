#ifndef _MSG_LOGIN_REQUEST_H_
#define _MSG_LOGIN_REQUEST_H_
#pragma once

//----------------------------------------------------------//
// MSGLOGINREQUEST.H
//----------------------------------------------------------//
//-- Description			
// A login request message sent by the client.
//----------------------------------------------------------//


#include "Types.h"
#include "FixedString.h"
#include "Message.h"


//----------------------------------------------------------//
// DEFINES
//----------------------------------------------------------//


#define MSG_LOGIN_REQUEST_MAX_NAME_SIZE			(128)
#define MSG_LOGIN_REQUEST_MAX_PASSWORD_SIZE		(128)


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


class CMsgLoginRequest : public CMessage
{
		DECLARE_MESSAGE_REGISTRAR(CMsgLoginRequest, 'logn');

	public:

		CMsgLoginRequest(const s8* strName, const s8* strPassword);
		CMsgLoginRequest();
		virtual ~CMsgLoginRequest();

		//-- CMessage
		virtual size_t									Serialize(CSerializer& serializer);

		const s8*										GetName(void) const;
		void											SetName(const s8* strBuffer);

		const s8*										GetPassword(void) const;
		void											SetPassword(const s8* strBuffer);

	private:

		FixedString<MSG_LOGIN_REQUEST_MAX_NAME_SIZE>		m_strName;
		FixedString<MSG_LOGIN_REQUEST_MAX_PASSWORD_SIZE>	m_strPassword;
		u8												m_nStrNameLength;
		u8												m_nStrPasswordLength;
};


//----------------------------------------------------------//
// EXTERNALS
//----------------------------------------------------------//

//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//

#endif //_MSG_LOGIN_REQUEST_H_