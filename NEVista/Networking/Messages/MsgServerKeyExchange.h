#ifndef _MSG_SERVER_KEYEXCHANGE_H_
#define _MSG_SERVER_KEYEXCHANGE_H_
#pragma once

//----------------------------------------------------------//
// MSGSERVERKEYEXCHANGE.H
//----------------------------------------------------------//
//-- Description			
// A key exchange message.
// Message containing the server half of an encryption key.
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


class CMsgServerKeyExchange : public CMessage
{
		DECLARE_MESSAGE_REGISTRAR(CMsgServerKeyExchange, 'skey');

	public:

		CMsgServerKeyExchange();
		virtual ~CMsgServerKeyExchange();

		//-- CMessage
		virtual size_t			Serialize(CSerializer& serializer);

		void					SetKey(u8 nServerKey);
		u8						GetKey(void) const;

	private:

		u8						m_nServerKey;

};


//----------------------------------------------------------//
// EXTERNALS
//----------------------------------------------------------//

//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//

#endif //_MSG_SERVER_KEYEXCHANGE_H_