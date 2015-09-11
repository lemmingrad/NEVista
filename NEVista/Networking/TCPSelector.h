#ifndef _TCPSELECTOR_H_
#define _TCPSELECTOR_H_
#pragma once

//----------------------------------------------------------//
// TCPSELECTOR.H
//----------------------------------------------------------//
//-- Description			
// Wrapper around poll/select for driving events on a
// list of TCPConnections.
//----------------------------------------------------------//


#include "Types.h"
#include "Network.h"
#include "SysSocket.h"
#include <map>


//----------------------------------------------------------//
// DEFINES
//----------------------------------------------------------//


//-- Define this to use poll instead of select.
#define TCPSELECTOR_USES_POLL


//----------------------------------------------------------//
// ENUMS
//----------------------------------------------------------//

//----------------------------------------------------------//
// FORWARD REFERENCES
//----------------------------------------------------------//

//----------------------------------------------------------//
// STRUCTS
//----------------------------------------------------------//

//----------------------------------------------------------//
// CLASSES
//----------------------------------------------------------//


class CTCPSelector
{
	public:

		struct Error
		{
			enum Enum
			{
				AlreadyRegistered		= 0x80000000,
				SelectFail				= 0x80000001,
				PollFail				= 0x80000002,

				BadFail					= -1,

				//-- Success
				Ok						= 0
			};
		};

		struct Event
		{
			enum Enum
			{
				Read		= BIT(0),
				Write		= BIT(1),
				Exception	= BIT(2)
			};
		};

		typedef s32	(*TEventCallback)(Event::Enum eEvent, void* pData);

		CTCPSelector();
		~CTCPSelector();

		Error::Enum						Update(void);

		Error::Enum						AddEventListener(SysSocket::Socket nSocket, TEventCallback cb, void* pData);
		Error::Enum						RemoveEventListener(SysSocket::Socket nSocket);

	private:

		struct EventListener
		{
			TEventCallback				m_pCallbackFunc;
			void*						m_pData;
		};

		typedef std::map<SysSocket::Socket, EventListener> TEventListenerMap;

		TEventListenerMap				m_EventListenerMap;

#		if defined(TCPSELECTOR_USES_POLL)

			SysSocket::PollFdSet		m_PollFdSet;

#		else

			SysSocket::FdSet			m_SelectFdSet;
			SysSocket::FdSet			m_ReadFdSet;
			SysSocket::FdSet			m_WriteFdSet;
			SysSocket::FdSet			m_ExceptionFdSet;

			s32							m_nLargestSocket;

#		endif
};


//----------------------------------------------------------//
// EXTERNALS
//----------------------------------------------------------//

//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//

#endif //_TCPSELECTOR_H_