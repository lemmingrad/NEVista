#ifndef _MSG_MOTD_H_
#define _MSG_MOTD_H_
#pragma once

#include "Types.h"
#include "SysString.h"
#include "Serialized.h"


#define MSG_MOTD_MAX_SIZE (980)


class CPacketSerializer;


class CMsgMotd : public CSerialized
{
	public:

		CMsgMotd();
		~CMsgMotd();

		//-- CSerialized
		virtual void						Serialize(CPacketSerializer& Serializer);

	private:

		FixedString<MSG_MOTD_MAX_SIZE>		m_strMotd;
		u16									m_nMotdLength;
};






#endif //_MSG_MOTD_H_