#ifndef _MSG_CLIENT_KEYEXCHANGE_H_
#define _MSG_CLIENT_KEYEXCHANGE_H_
#pragma once

#include "Types.h"
#include "SysString.h"


class CPacketSerializer;


class CMsgClientKeyExchange
{
	public:

		CMsgClientKeyExchange();
		~CMsgClientKeyExchange();

		void	Serialize(CPacketSerializer& Serailizer);

		void	SetKey(u16 nClientKey);
		u16		GetKey(void);

	private:

		u16				m_nClientKey;

};






#endif //_MSG_CLIENT_KEYEXCHANGE_H_