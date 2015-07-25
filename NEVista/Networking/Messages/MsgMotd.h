#ifndef _MSG_MOTD_H_
#define _MSG_MOTD_H_
#pragma once

#include "Types.h"
#include "FixedString.h"
#include "Message.h"


#define MSG_MOTD_MAX_SIZE (980)


class CSerializer;


class CMsgMotd : public CMessage
{
	public:

		CMsgMotd();
		virtual ~CMsgMotd();

		//-- CMessage
		virtual size_t						Serialize(CSerializer& serializer);

		void Set(const s8* strBuffer)
		{
			m_strMotd.Set(strBuffer);
		}

	private:

		FixedString<MSG_MOTD_MAX_SIZE>		m_strMotd;
		u16									m_nMotdLength;
};






#endif //_MSG_MOTD_H_