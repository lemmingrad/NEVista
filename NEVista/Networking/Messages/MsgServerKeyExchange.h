#ifndef _MSG_SERVER_KEYEXCHANGE_H_
#define _MSG_SERVER_KEYEXCHANGE_H_
#pragma once

#include "Types.h"
#include "SysString.h"


class CPacketSerializer;


class CMsgServerKeyExchange
{
	public:

		CMsgServerKeyExchange();
		~CMsgServerKeyExchange();

		void	Serialize(CPacketSerializer& Serailizer);

		void	SetKey(u16 nServerKey);
		u16		GetKey(void);

	private:

		u16				m_nServerKey;

};






#endif //_MSG_SERVER_KEYEXCHANGE_H_