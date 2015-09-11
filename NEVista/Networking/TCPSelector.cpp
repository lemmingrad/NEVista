//----------------------------------------------------------//
// TCPSELECTOR.CPP
//----------------------------------------------------------//
//-- Description			
// Wrapper around select or poll, to generate event callbacks.
//----------------------------------------------------------//


#include "TCPSelector.h"
#include "Types.h"
#include "Network.h"
#include "SysSocket.h"
#include <map>


//----------------------------------------------------------//
// DEFINES
//----------------------------------------------------------//

//----------------------------------------------------------//
// GLOBALS
//----------------------------------------------------------//

//----------------------------------------------------------//
// CTCPSelector::CTCPSelector
//----------------------------------------------------------//
//-- Description			
//----------------------------------------------------------//
CTCPSelector::CTCPSelector()
{
	m_EventListenerMap.clear();

#	if defined(TCPSELECTOR_USES_POLL)
	
		m_PollFdSet.clear();

#	else

		FD_ZERO(&m_SelectFdSet);

#	endif
}


//----------------------------------------------------------//
// CTCPSelector::~CTCPSelector
//----------------------------------------------------------//
//-- Description			
//----------------------------------------------------------//
CTCPSelector::~CTCPSelector()
{
	m_EventListenerMap.clear();

#	if defined(TCPSELECTOR_USES_POLL)
	
		m_PollFdSet.clear();

#	else

		FD_ZERO(&m_SelectFdSet);
		m_nLargestSocket = 0;

#	endif
}


//----------------------------------------------------------//
// CTCPSelector::Update
//----------------------------------------------------------//
//-- Description	
// Updates the selector, calling poll/select and generating
// callbacks on registered sockets.
//----------------------------------------------------------//
CTCPSelector::Error::Enum CTCPSelector::Update(void)
{
#	if defined(TCPSELECTOR_USES_POLL)
	
		s32 nError = SysSocket::Poll(&m_PollFdSet[0], m_PollFdSet.size(), 0);
		if (SYS_SOCKET_ERROR == nError)
		{
			//-- Error occurred in poll
			return Error::PollFail;
		}
		if (IS_ZERO(nError))
		{
			//-- Nothing to process
			return Error::Ok;
		}

		for (SysSocket::PollFdSet::iterator cit = m_PollFdSet.begin(); cit != m_PollFdSet.end(); ++cit)
		{
			TEventListenerMap::iterator citelm = m_EventListenerMap.find(cit->fd);
			if (citelm == m_EventListenerMap.end())
			{
				continue;
			}

			EventListener& eventListener = citelm->second;

			if (eventListener.m_pCallbackFunc)
			{
				if (TEST_FLAG(cit->revents, POLLIN))
				{
					eventListener.m_pCallbackFunc(Event::Read, eventListener.m_pData);
				}
				if (TEST_FLAG(cit->revents, POLLOUT))
				{
					eventListener.m_pCallbackFunc(Event::Write, eventListener.m_pData);
				}
				if (TEST_FLAG(cit->revents, POLLERR))
				{
					eventListener.m_pCallbackFunc(Event::Exception, eventListener.m_pData);			
				}
			}
		}

#	else

		m_ReadFdSet = m_SelectFdSet;
		m_WriteFdSet = m_SelectFdSet;
		m_ExceptionFdSet = m_SelectFdSet;

		SysSocket::Timeval tv = {0, 0};

		s32 nError = SysSocket::Select(m_nLargestSocket + 1, &m_ReadFdSet, &m_WriteFdSet, &m_ExceptionFdSet, &tv);
		if (SYS_SOCKET_ERROR == nError)
		{
			//-- Error occurred in select.
			return Error::SelectFail;
		}

		for (TEventListenerMap::const_iterator cit = m_EventListenerMap.begin(); cit != m_EventListenerMap.end(); ++cit)
		{
			EventListener& eventListener = cit->second;

			if (eventListener.m_pCallbackFunc)
			{
				if (FD_ISSET(cit->first, &m_ReadFdSet))
				{
					eventListener.m_pCallbackFunc(Event::Read, eventListener.m_pData);
				}
				if (FD_ISSET(cit->first, &m_WriteFdSet))
				{
					eventListener.m_pCallbackFunc(Event::Write, eventListener.m_pData);
				}
				if (FD_ISSET(cit->first, &m_ExceptionFdSet))
				{
					eventListener.m_pCallbackFunc(Event::Exception, eventListener.m_pData);
				}
			}
		}

#	endif

	return Error::Ok;
}


//----------------------------------------------------------//
// CTCPSelector::AddEventListener
//----------------------------------------------------------//
//-- Description			
//----------------------------------------------------------//
CTCPSelector::Error::Enum CTCPSelector::AddEventListener(SysSocket::Socket nSocket, CTCPSelector::TEventCallback cb, void* pData)
{
	TEventListenerMap::const_iterator cit = m_EventListenerMap.find(nSocket);
	if (m_EventListenerMap.end() != cit)
	{
		return Error::AlreadyRegistered;
	}

	EventListener eventListener;
	eventListener.m_pCallbackFunc = cb;
	eventListener.m_pData = pData;

	m_EventListenerMap[nSocket] = eventListener;

#	if defined(TCPSELECTOR_USES_POLL)
	
		SysSocket::PollFd pollFd;
		pollFd.fd = nSocket;
		pollFd.events = POLLIN | POLLOUT;
		m_PollFdSet.push_back(pollFd);

#	else

		FD_SET(nSocket, &m_SelectFdSet);
		m_nLargestSocket = MAX(m_nLargestSocket, (s32)nSocket);
		m_nLargestSocket = MIN(m_nLargestSocket, FD_SETSIZE);

#	endif

	return Error::Ok;
}


//----------------------------------------------------------//
// CTCPSelector::RemoveSocketListener
//----------------------------------------------------------//
//-- Description			
//----------------------------------------------------------//
CTCPSelector::Error::Enum CTCPSelector::RemoveEventListener(SysSocket::Socket nSocket)
{
	m_EventListenerMap.erase(nSocket);

#	if defined(TCPSELECTOR_USES_POLL)
	
		for (SysSocket::PollFdSet::iterator it = m_PollFdSet.begin(); it != m_PollFdSet.end(); ++it)
		{
			if (nSocket == it->fd)
			{
				m_PollFdSet.erase(it);
				break;
			}
		}

#	else

		FD_CLR(nSocket, &m_SelectFdSet);

#	endif
		
	return Error::Ok;
}


//----------------------------------------------------------//
// EXTERNALS
//----------------------------------------------------------//

//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//
