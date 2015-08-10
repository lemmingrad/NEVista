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


class CSerializer;


//----------------------------------------------------------//
// STRUCTS
//----------------------------------------------------------//

//----------------------------------------------------------//
// CLASSES
//----------------------------------------------------------//


class CMsgClientKeyExchange : public CMessage
{
	public:

		CMsgClientKeyExchange();
		virtual ~CMsgClientKeyExchange();

		//-- CMessage
		virtual size_t			Serialize(CSerializer& serializer);

		void					SetKey(u16 nClientKey);
		u16						GetKey(void) const;

	private:

		u16						m_nClientKey;

};


//----------------------------------------------------------//
// EXTERNALS
//----------------------------------------------------------//

//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//

#endif //_MSG_CLIENT_KEYEXCHANGE_H_