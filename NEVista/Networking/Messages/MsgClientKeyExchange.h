#ifndef _MSG_CLIENT_KEYEXCHANGE_H_
#define _MSG_CLIENT_KEYEXCHANGE_H_
#pragma once

//----------------------------------------------------------//
// MSGCLIENTKEYEXCHANGE.H
//----------------------------------------------------------//
//-- Description			
// A key exchange message.
// Message containing the client half of an encryption key.
//----------------------------------------------------------//


#include "Types.h"
#include "Message.h"


//----------------------------------------------------------//
// DEFINES
//----------------------------------------------------------//

//----------------------------------------------------------//
// ENUMS
//----------------------------------------------------------//

//----------------------------------------------------------//
// FORWARD REFERENCES
//----------------------------------------------------------//


class ISerializer;


//----------------------------------------------------------//
// STRUCTS
//----------------------------------------------------------//

//----------------------------------------------------------//
// CLASSES
//----------------------------------------------------------//


class CMsgClientKeyExchange : public CMessage
{
		DECLARE_MESSAGE_REGISTRAR(CMsgClientKeyExchange, 'ckey');

	public:

		CMsgClientKeyExchange();
		virtual ~CMsgClientKeyExchange();

		//-- CMessage
		virtual size_t			Serialize(ISerializer& serializer);

		void					SetKey(u8 nClientKey);
		u8						GetKey(void) const;

	private:

		u8						m_nClientKey;

};


//----------------------------------------------------------//
// EXTERNALS
//----------------------------------------------------------//

//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//

#endif //_MSG_CLIENT_KEYEXCHANGE_H_