#ifndef _MSG_CLIENT_KEYEXCHANGE_H_
#define _MSG_CLIENT_KEYEXCHANGE_H_
#pragma once

#include "Types.h"
#include "SysString.h"
#include "Message.h"


class CSerializer;


class CMsgClientKeyExchange : public CMessage
{
	public:

		CMsgClientKeyExchange();
		virtual ~CMsgClientKeyExchange();

		//-- CMessage
		virtual size_t			Serialize(CSerializer& serializer);

		void					SetKey(u16 nClientKey);
		u16						GetKey(void);

	private:

		u16						m_nClientKey;

};






#endif //_MSG_CLIENT_KEYEXCHANGE_H_