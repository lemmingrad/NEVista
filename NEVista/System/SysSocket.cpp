//----------------------------------------------------------//
// SYSSOCKET.CPP
//----------------------------------------------------------//
//-- Description
// Wrapper for socket functions to hide implementation of 
// a socket on a platform.
//----------------------------------------------------------//


#include "SysSocket.h"
#include "Types.h"
#include "SysMemory.h"
#include "SysDebugLog.h"

#if defined(SYSSOCKET_USES_WINSOCK)
	
#	include <WS2tcpip.h>
#	include <winsock2.h>

#else

//-- Define this to use ioctl instead of fcntl on non-winsock platforms
//#	define SYSSOCKET_USES_IOCTL

//-- Define this to use inet_pton
#define SYSSOCKET_USES_INET_PTON
//-- Define this to use inet_ntop
#define SYSSOCKET_USES_INET_NTOP

#	include <errno.h>
#	include <poll.h>
#	include <sys/types.h>
#	include <sys/socket.h>
#	include <arpa/inet.h>
#	include <netinet/in.h>
#	include <netdb.h>

#	if defined(SYSSOCKET_USES_IOCTL)

#		include <sys/ioctl.h>

#	else

#		include <fcntl.h>

#	endif //SYSSOCKET_USES_IOCTL

#endif //SYSSOCKET_USES_WINSOCK


//----------------------------------------------------------//
// DEFINES
//----------------------------------------------------------//

//----------------------------------------------------------//
// GLOBALS
//----------------------------------------------------------//


#if defined(SYSSOCKET_USES_WINSOCK)
	const SysSocket::Socket SysSocket::INVALID_SOCK = (SysSocket::Socket)INVALID_SOCKET;
#else
	const SysSocket::Socket SysSocket::INVALID_SOCK = (SysSocket::Socket)-1;
#endif //SYSSOCKET_USES_WINSOCK


//----------------------------------------------------------//
//
//----------------------------------------------------------//
//-- Description
//----------------------------------------------------------//
void SysSocket::SystemInitialise(void)
{
#if defined(SYSSOCKET_USES_WINSOCK)

	WSADATA wsaData;	

	gDebugLog.Printf("SysSocket::SystemInitialise:");
	SCOPED_LOG_INDENT(gDebugLog);

	if (SYS_SOCKET_NO_ERROR == WSAStartup(MAKEWORD(2,2), &wsaData))
	{
		//-- Success
		gDebugLog.Printf("Desc: %s", wsaData.szDescription);
		gDebugLog.Printf("Status: %s", wsaData.szSystemStatus);
		return;
	}

	//-- Failed.
	gDebugLog.Printf("WSAStartup failed.");

#endif //SYSSOCKET_USES_WINSOCK
}


//----------------------------------------------------------//
//
//----------------------------------------------------------//
//-- Description
//----------------------------------------------------------//
void SysSocket::SystemShutdown(void)
{
#if defined(SYSSOCKET_USES_WINSOCK)

	WSACleanup();

#endif //SYSSOCKET_USES_WINSOCK
}


//----------------------------------------------------------//
//
//----------------------------------------------------------//
//-- Description
//----------------------------------------------------------//
SysSocket::Address::Address(const SysSocket::SockAddr* pAddr)
{
	SysMemory::Memclear(&m_addr, sizeof(m_addr));

	if (IS_PTR(pAddr))
	{
		switch (pAddr->sa_family)
		{
			case AF_INET:
			{
				SysMemory::Memcpy(&m_addr, sizeof(m_addr), pAddr, sizeof(SysSocket::SockAddrIn));
			}
			break;
			case AF_INET6:
			{
				SysMemory::Memcpy(&m_addr, sizeof(m_addr), pAddr, sizeof(SysSocket::SockAddrIn6));
			}
			break;
			default:
			break;
		}
	}
}

SysSocket::Address::Address()
{
	SysMemory::Memclear(&m_addr, sizeof(m_addr));
}


//----------------------------------------------------------//
//
//----------------------------------------------------------//
//-- Description
//----------------------------------------------------------//
SysSocket::Address::~Address()
{
}


//----------------------------------------------------------//
//
//----------------------------------------------------------//
//-- Description
//----------------------------------------------------------//
SysSocket::SockAddr* SysSocket::Address::GetSockAddr(void)
{
	return (SysSocket::SockAddr*)&m_addr;
}


//----------------------------------------------------------//
//
//----------------------------------------------------------//
//-- Description
//----------------------------------------------------------//
size_t* SysSocket::Address::GetSockAddrSize(void)
{
	SysSocket::SockAddr* pAddr = GetSockAddr();
	switch (pAddr->sa_family)
	{
		case AF_INET:
		{
			m_nAddrSize = sizeof(SysSocket::SockAddrIn);
		}
		break;
		case AF_INET6:
		{
			m_nAddrSize = sizeof(SysSocket::SockAddrIn6);
		}
		break;
		default:
		{
			m_nAddrSize = sizeof(SysSocket::SockAddrStorage);
		}
		break;
	}

	return &m_nAddrSize;
}


//----------------------------------------------------------//
//
//----------------------------------------------------------//
//-- Description
//----------------------------------------------------------//
const s8* SysSocket::Address::GetIPAddress(s8* strBuffer, size_t nStrBufferSize)
{
	SysSocket::SockAddr* pAddr = GetSockAddr();
	switch (pAddr->sa_family)
	{
		case AF_INET:
		{
			SysSocket::SockAddrIn* pAddrIn = (SysSocket::SockAddrIn*)pAddr;
			return SysSocket::InetNToP(AF_INET, (const SysSocket::InAddr*)&pAddrIn->sin_addr, sizeof(pAddrIn->sin_addr), strBuffer, nStrBufferSize);
		}
		break;
		case AF_INET6:
		{
			SysSocket::SockAddrIn6* pAddrIn = (SysSocket::SockAddrIn6*)pAddr;
			return SysSocket::InetNToP(AF_INET6, (const SysSocket::In6Addr*)&pAddrIn->sin6_addr, sizeof(pAddrIn->sin6_addr), strBuffer, nStrBufferSize);
		}
		break;
		default:
		break;
	}

	return NULL;
}


//----------------------------------------------------------//
//
//----------------------------------------------------------//
//-- Description
//----------------------------------------------------------//
u16 SysSocket::Address::GetPort(void)
{
	SysSocket::SockAddr* pAddr = GetSockAddr();
	switch (pAddr->sa_family)
	{
		case AF_INET:
		{
			SysSocket::SockAddrIn* pAddrIn = (SysSocket::SockAddrIn*)pAddr;
			return SysSocket::Ntohs(pAddrIn->sin_port);
		}
		break;
		case AF_INET6:
		{
			SysSocket::SockAddrIn6* pAddrIn = (SysSocket::SockAddrIn6*)pAddr;
			return SysSocket::Ntohs(pAddrIn->sin6_port);
		}
		break;
		default:
		break;
	}
	
	return 0;
}


//----------------------------------------------------------//
//
//----------------------------------------------------------//
//-- Description
//----------------------------------------------------------//
s32 SysSocket::GetInfo(const s8* strName, const s8* strPort, const SysSocket::AddrInfo* pHints, SysSocket::AddrInfo** pResults)
{
	return ::getaddrinfo(strName, strPort, pHints, pResults);
}


//----------------------------------------------------------//
//
//----------------------------------------------------------//
//-- Description
//----------------------------------------------------------//
void SysSocket::FreeInfo(SysSocket::AddrInfo* pResults)
{
	::freeaddrinfo(pResults);
}
		
		
//----------------------------------------------------------//
//
//----------------------------------------------------------//
//-- Description
//----------------------------------------------------------//
SysSocket::Socket SysSocket::OpenSocket(s32 nAF, s32 nType, s32 nProtocol)
{
	return ::socket(nAF, nType, nProtocol);
}


//----------------------------------------------------------//
//
//----------------------------------------------------------//
//-- Description
//----------------------------------------------------------//
s32 SysSocket::Bind(SysSocket::Socket nSocket, const SysSocket::SockAddr* pAddress, size_t nAddressSize)
{
	return ::bind(nSocket, pAddress, nAddressSize);
}


//----------------------------------------------------------//
//
//----------------------------------------------------------//
//-- Description
//----------------------------------------------------------//
s32 SysSocket::Connect(SysSocket::Socket nSocket, const SysSocket::SockAddr* pAddress, size_t nAddressSize)
{
	s32 nRet = ::connect(nSocket, pAddress, nAddressSize);
	if (SYS_SOCKET_ERROR == nRet)
	{
#		if defined(SYSSOCKET_USES_WINSOCK)

			int er = WSAGetLastError();
			switch (er)
			{
				case WSAEWOULDBLOCK: 
				case WSAEALREADY:
				case WSAEINVAL:
				{
					nRet = SYS_SOCKET_IN_PROGRESS;
				}
				break;
				case WSAEISCONN:
				{
					nRet = SYS_SOCKET_NO_ERROR;
				}
				break;
				default:
				break;
			}

#		else

			switch (errno)
			{
				case EINPROGRESS:
				case EALREADY:
				{
					nRet = SYS_SOCKET_IN_PROGRESS;
				}
				break;
				case EISCONN:
				{
					nRet = SYS_SOCKET_NO_ERROR;
				}
				break;	
				default:
				break;
			}

#		endif //SYSSOCKET_USES_WINSOCK
	}

	return nRet;
}


//----------------------------------------------------------//
//
//----------------------------------------------------------//
//-- Description
//----------------------------------------------------------//
s32 SysSocket::Listen(SysSocket::Socket nSocket, s32 nBacklog)
{
	return ::listen(nSocket, nBacklog);
}


///----------------------------------------------------------//
//
//----------------------------------------------------------//
//-- Description
//----------------------------------------------------------//
SysSocket::Socket SysSocket::Accept(SysSocket::Socket nSocket, SysSocket::SockAddr* pAddress, size_t* pAddressSize)
{
#if defined(SYSSOCKET_USES_WINSOCK)

	return ::accept(nSocket, pAddress, (int*)pAddressSize);

#else

	return ::accept(nSocket, pAddress, (socklen_t*)pAddressSize);

#endif
}


//----------------------------------------------------------//
//
//----------------------------------------------------------//
//-- Description
//----------------------------------------------------------//
s32 SysSocket::Select(s32 nLargestFD, 
						SysSocket::FdSet* pReadFDs, SysSocket::FdSet* pWriteFDs, SysSocket::FdSet* pExceptFDs, 
						SysSocket::Timeval* pTimeout)
{
	return ::select(nLargestFD, pReadFDs, pWriteFDs, pExceptFDs, pTimeout);
}


//----------------------------------------------------------//
// SysSocket::Poll
//----------------------------------------------------------//
//-- Description
// Prefered alternative to Select().
// WARNING: Will be really slow on Windows, so avoid it there.
// TODO: Replace with WSAPoll on Windows > Vista, eventually.
//----------------------------------------------------------//
s32	SysSocket::Poll(SysSocket::PollFd* pFds, u32 nFds, s32 nTimeout)
{
#if defined(SYSSOCKET_USES_WINSOCK)

	FdSet ReadSet;
	FdSet WriteSet;
	FdSet ExceptionSet;
	Timeval tv;
	
	if (nFds > 0)
	{
		FD_ZERO(&ReadSet);
		FD_ZERO(&WriteSet);
		FD_ZERO(&ExceptionSet);

		//-- nTimeout is in milliseonds.
		//-- Need to convert to seconds and microseconds.
		if (IS_ZERO(nTimeout))
		{
			tv.tv_sec = 0;
			tv.tv_usec = 0;
		}
		else if (nTimeout > 0)
		{
			tv.tv_sec = nTimeout / 1000;
			tv.tv_usec = (nTimeout % 1000) * 1000;
		}

		s32 nLargestFD = 0;

		for (u32 i = 0; i < nFds; ++i)
		{	
			SysSocket::Socket fd = pFds[i].fd;

			if (SysSocket::INVALID_SOCK == fd)
			{
				continue;
			}

			s16 events = pFds[i].events;

			if (TEST_FLAG(events, POLLIN))
			{
				FD_SET(fd, &ReadSet);
			}
			if (TEST_FLAG(events, POLLOUT))
			{
				FD_SET(fd, &WriteSet);
			}
			FD_SET(fd, &ExceptionSet);
			pFds[i].revents = 0;

			nLargestFD = MAX(nLargestFD, (s32)fd);
		}

		nLargestFD = MIN(nLargestFD, FD_SETSIZE);

		s32 nRes = Select(nLargestFD, &ReadSet, &WriteSet, &ExceptionSet, (nTimeout >= 0) ? &tv : NULL);
		if (nRes > 0)
		{
			nRes = 0;
			for (u32 i = 0; i < nFds; ++i)
			{	
				SysSocket::Socket fd = pFds[i].fd;

				if (FD_ISSET(fd, &ReadSet))
				{
					SET_FLAG(pFds[i].revents, POLLIN);
				}
				if (FD_ISSET(fd, &WriteSet))
				{
					SET_FLAG(pFds[i].revents, POLLOUT);
				}
				if (FD_ISSET(fd, &ExceptionSet))
				{
					SET_FLAG(pFds[i].revents, POLLERR);
				}
				if (IS_NOT_ZERO(pFds[i].revents))
				{
					nRes++;
				}
			}
		}

		return nRes;
	}

	return SYS_SOCKET_ERROR;

#else

	return ::poll(pFds, nFds, nTimeout);

#endif //SYSSOCKET_USES_WINSOCK
}



//----------------------------------------------------------//
//
//----------------------------------------------------------//
//-- Description
//----------------------------------------------------------//
s32 SysSocket::Shutdown(SysSocket::Socket nSocket, s32 nHow)
{
	return ::shutdown(nSocket, nHow);
}


//----------------------------------------------------------//
//
//----------------------------------------------------------//
//-- Description
//----------------------------------------------------------//
s32 SysSocket::CloseSocket(SysSocket::Socket nSocket)
{
#if defined(SYSSOCKET_USES_WINSOCK)

	return ::closesocket(nSocket);

#else

	return ::close(nSocket);

#endif //SYSSOCKET_USES_WINSOCK
}


//----------------------------------------------------------//
//
//----------------------------------------------------------//
//-- Description
//----------------------------------------------------------//
s32 SysSocket::SetNonblocking(SysSocket::Socket nSocket, bool bIsNonblocking)
{
#if defined(SYSSOCKET_USES_WINSOCK)

	s32 nIsNonblocking = bIsNonblocking;
	return ::ioctlsocket(nSocket, FIONBIO, (u_long*)&nIsNonblocking);

#else

#	if defined(SYSSOCKET_USES_IOCTL)
	
		s8 nIsNonblocking = bIsNonblocking;
		return ::ioctl(nSocket, FIONBIO, (char*)&nIsNonblocking);

#	else

		s32 nFlags = ::fcntl(nSocket, F_GETFL, 0);
		if (nFlags < 0) 
		{
			return SYS_SOCKET_ERROR;
		}

		if (IS_TRUE(bIsNonblocking))
		{
			SET_FLAG(nFlags, O_NONBLOCK);
		}else
		{
			CLEAR_FLAG(nFlags, O_NONBLOCK);
		}
				
		return ::fcntl(nSocket, F_SETFL, nFlags);

#	endif //SYSSOCKET_USES_IOCTL

#endif //SYSSOCKET_USES_WINSOCK
}


//----------------------------------------------------------//
//
//----------------------------------------------------------//
//-- Description
//----------------------------------------------------------//
s32 SysSocket::SetReusable(SysSocket::Socket nSocket, bool bIsReusable)
{
	s32 nIsReusable = bIsReusable;
	return SetSockOpt(nSocket, SOL_SOCKET, SO_REUSEADDR, (const void*)&nIsReusable, sizeof(s32));
}


//----------------------------------------------------------//
//
//----------------------------------------------------------//
//-- Description
//----------------------------------------------------------//
s32 SysSocket::SetSockOpt(SysSocket::Socket nSocket, s32 nLevel, s32 nOption, const void* pOptVal, size_t nOptValSize)
{
#if defined(SYSSOCKET_USES_WINSOCK)

	return ::setsockopt(nSocket, nLevel, nOption, (const s8*)pOptVal, (int)nOptValSize);

#else

	return ::setsockopt(nSocket, nLevel, nOption, pOptVal, (socklen_t)nOptValSize);

#endif //SYSSOCKET_USES_WINSOCK
}


//----------------------------------------------------------//
//
//----------------------------------------------------------//
//-- Description
//----------------------------------------------------------//
s32 SysSocket::GetSockOpt(SysSocket::Socket nSocket, s32 nLevel, s32 nOption, void* pOptVal, size_t* pOptValSize)
{
#if defined(SYSSOCKET_USES_WINSOCK)

	return ::getsockopt(nSocket, nLevel, nOption, (s8*)pOptVal, (int*)pOptValSize);

#else

	return ::getsockopt(nSocket, nLevel, nOption, pOptVal, (socklen_t*)pOptValSize);

#endif //SYSSOCKET_USES_WINSOCK
}


//----------------------------------------------------------//
//
//----------------------------------------------------------//
//-- Description
//----------------------------------------------------------//
u16 SysSocket::Ntohs(u16 nValue)
{
	return ::ntohs(nValue);
}


//----------------------------------------------------------//
//
//----------------------------------------------------------//
//-- Description
//----------------------------------------------------------//
u16 SysSocket::Htons(u16 nValue)
{
	return ::htons(nValue);
}


//----------------------------------------------------------//
//
//----------------------------------------------------------//
//-- Description
//----------------------------------------------------------//
u32 SysSocket::Ntohl(u32 nValue)
{
	return ::ntohl(nValue);
}


//----------------------------------------------------------//
//
//----------------------------------------------------------//
//-- Description
//----------------------------------------------------------//
u32 SysSocket::Htonl(u32 nValue)
{
	return ::htonl(nValue);
}


//----------------------------------------------------------//
//
//----------------------------------------------------------//
//-- Description
//----------------------------------------------------------//
s32 SysSocket::InetPToN(s32 nType, const s8* strSrc, void* pDest, size_t nDestSize)
{
	if ( IS_TRUE(SysString::IsEmpty(strSrc))
		|| IS_NULL_PTR(pDest) )
	{
		return -1;
	}

	if ( (AF_INET == nType)
		&& (nDestSize < sizeof(InAddr)) )
	{
		return -1;
	}
	else if ( (AF_INET6 == nType)
		&& (nDestSize < sizeof(In6Addr)) )
	{
		return -1;
	}

#if defined(SYSSOCKET_USES_INET_PTON)

	if ( (AF_INET != nType) 
		&& (AF_INET6 != nType) )
	{
		return -1;
	}

	//-- returns 0 if invalid string buffer.
	//-- returns -1 for other errors.
	//-- returns 1 for success.

#	if	defined(SYSSOCKET_USES_WINSOCK)

		return InetPton(nType, strSrc, pDest);

#	else

		return ::inet_pton(nType, strSrc, pDest);

#	endif

#else

	if (AF_INET != nType) 
	{
		return -1;
	}

	u32 nVal = ::inet_addr(strSrc);
	if (INADDR_NONE == nVal)
	{
		return 0;
	}
	
	InAddr* pAddr = (InAddr*)pDest;
	pAddr->s_addr = nVal;

	return 1;

#endif
}


//----------------------------------------------------------//
//
//----------------------------------------------------------//
//-- Description
//----------------------------------------------------------//
const s8* SysSocket::InetNToP(s32 nType, const void* pSrc, size_t nSrcSize, s8* strDest, size_t nDestSize)
{
	if ( IS_NULL_PTR(pSrc)
		|| IS_NULL_PTR(strDest) )
	{
		return NULL;
	}

	if (AF_INET == nType)
	{
		if ( (nSrcSize < sizeof(InAddr))
			|| (nDestSize < INET_ADDRSTRLEN) )
		{
			return NULL;
		}
	}
	else if (AF_INET6 == nType)
	{
		if ( (nSrcSize < sizeof(In6Addr))
			|| (nDestSize < INET6_ADDRSTRLEN) )
		{
			return NULL;
		}
	}

#if defined(SYSSOCKET_USES_INET_NTOP)

	if ( (AF_INET != nType) 
		&& (AF_INET6 != nType) )
	{
		return NULL;
	}

	//-- returns NULL if error.
	//-- returns strDest for success.

#	if	defined(SYSSOCKET_USES_WINSOCK)

		return InetNtop(nType, pSrc, strDest, nDestSize);

#	else

		return ::inet_ntop(nType, pSrc, strDest, (socklen_t)nDestSize);

#	endif

#else

	if (AF_INET != nType) 
	{
		return NULL;
	}

	s8* pAddress = ::inet_ntoa(*((InAddr*)pSrc));
	if (IS_PTR(pAddress))
	{
		SysString::Strcpy(strDest, nDestSize, pAddress);
		return (const s8*)strDest;
	}

	return NULL;

#endif
}


//----------------------------------------------------------//
//
//----------------------------------------------------------//
//-- Description
//----------------------------------------------------------//
s32 SysSocket::Send(SysSocket::Socket nSocket, const s8* pBuffer, size_t nBufferSize)
{
	s32 nRet = send(nSocket, pBuffer, nBufferSize, 0);
	if (SYS_SOCKET_ERROR == nRet)
	{

#if defined(SYSSOCKET_USES_WINSOCK)

		if (WSAEWOULDBLOCK == WSAGetLastError())
		{
			nRet = SYS_SOCKET_WOULD_BLOCK;
		}

#else

		if ( (EWOULDBLOCK == errno)
			|| (EAGAIN == errno) )
		{
			nRet = SYS_SOCKET_WOULD_BLOCK;
		}

#endif //SYSSOCKET_USES_WINSOCK

	}

	return nRet;
}


//----------------------------------------------------------//
//
//----------------------------------------------------------//
//-- Description
//----------------------------------------------------------//
s32 SysSocket::Sendto(SysSocket::Socket nSocket, const s8* pBuffer, size_t nBufferSize, 
					  const SysSocket::SockAddr* pAddr, size_t nAddrSize)
{
	s32 nRet = sendto(nSocket, pBuffer, nBufferSize, 0, pAddr, nAddrSize);
	if (SYS_SOCKET_ERROR == nRet)
	{

#if defined(SYSSOCKET_USES_WINSOCK)

		if (WSAEWOULDBLOCK == WSAGetLastError())
		{
			nRet = SYS_SOCKET_WOULD_BLOCK;
		}
#else

		if ( (EWOULDBLOCK == errno)
			|| (EAGAIN == errno) )
		{
			nRet = SYS_SOCKET_WOULD_BLOCK;
		}

#endif //SYSSOCKET_USES_WINSOCK

	}

	return nRet;
}


//----------------------------------------------------------//
//
//----------------------------------------------------------//
//-- Description
//----------------------------------------------------------//
s32 SysSocket::Recv(SysSocket::Socket nSocket, s8* pBuffer, size_t nBufferSize)
{
	s32 nRet = recv(nSocket, pBuffer, nBufferSize, 0);
	if (IS_ZERO(nRet))
	{
		nRet = SYS_SOCKET_RECV_ZERO;
	}
	else if (SYS_SOCKET_ERROR == nRet)
	{

#if defined(SYSSOCKET_USES_WINSOCK)

		int e = WSAGetLastError();
		if (WSAEWOULDBLOCK == e)//WSAGetLastError())
		{
			nRet = SYS_SOCKET_WOULD_BLOCK;
		}

#else 

		if ( (EWOULDBLOCK == errno)
			|| (EAGAIN == errno) )
		{
			nRet = SYS_SOCKET_WOULD_BLOCK;
		}

#endif //SYSSOCKET_USES_WINSOCK

	}

	return nRet;
}


//----------------------------------------------------------//
//
//----------------------------------------------------------//
//-- Description
//----------------------------------------------------------//
s32 SysSocket::Recvfrom(SysSocket::Socket nSocket, s8* pBuffer, size_t nBufferSize, 
						SysSocket::SockAddr* pAddr, size_t* pAddrSize)
{
#if defined(SYSSOCKET_USES_WINSOCK)

	s32 nRet = recvfrom(nSocket, pBuffer, nBufferSize, 0, pAddr, (int*)pAddrSize);
	if (IS_ZERO(nRet))
	{
		nRet = SYS_SOCKET_RECV_ZERO;
	}
	else if (SYS_SOCKET_ERROR == nRet)
	{
		if (WSAEWOULDBLOCK == WSAGetLastError())
		{
			nRet = SYS_SOCKET_WOULD_BLOCK;
		}
	}

#else 

	s32 nRet = recvfrom(nSocket, pBuffer, nBufferSize, 0, pAddr, (socklen_t*)pAddrSize);
	if (IS_ZERO(nRet))
	{
		nRet = SYS_SOCKET_RECV_ZERO;
	}
	else if (SYS_SOCKET_ERROR == nRet)
	{
		if ( (EWOULDBLOCK == errno)
			|| (EAGAIN == errno) )
		{
			nRet = SYS_SOCKET_WOULD_BLOCK;
		}
	}

#endif //SYSSOCKET_USES_WINSOCK

	return nRet;
}


//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//
