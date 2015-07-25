#ifndef _MSG_SERVER_KEYEXCHANGE_H_
#define _MSG_SERVER_KEYEXCHANGE_H_
#pragma once

#include "Types.h"
#include "SysString.h"
#include "Message.h"


class CSerializer;


class CMsgServerKeyExchange : public CMessage
{
	public:

		CMsgServerKeyExchange();
		virtual ~CMsgServerKeyExchange();

		//-- CMessage
		virtual size_t			Serialize(CSerializer& serializer);

		void					SetKey(u16 nServerKey);
		u16						GetKey(void);

	private:

		u16						m_nServerKey;

};






#endif //_MSG_SERVER_KEYEXCHANGE_H_