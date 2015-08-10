//----------------------------------------------------------//
// SYSSOCKET.CPP
//----------------------------------------------------------//
//-- Description
// Wrapper for socket functions to hide implementation of 
// a socket on a platform.
//----------------------------------------------------------//


#include "SysSocket.h"
#include "Types.h"
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
			if (SYS_SOCKET_WOULD_BLOCK == er)
			{
				nRet = SYS_SOCKET_NO_ERROR;
			}

#		else

			if (EINPROGRESS == errno)
			{
				nRet = SYS_SOCKET_NO_ERROR;
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
		nFds = MIN(nFds, FD_SETSIZE);
		s32 nLargestFD = (s32)nFds + 1;

		FD_ZERO(&ReadSet);
		FD_ZERO(&WriteSet);
		FD_ZERO(&ExceptionSet);

		//-- nTimeout is in milliseonds.
		//-- Need to convert to seconds and microseconds.
		if (0 == nTimeout)
		{
			tv.tv_sec = 0;
			tv.tv_usec = 0;
		}
		else if (nTimeout > 0)
		{
			tv.tv_sec = nTimeout / 1000;
			tv.tv_usec = (nTimeout % 1000) * 1000;
		}

		s32 something=1;

		for (u32 i = 0; i < nFds; ++i)
		{	
			if (TEST_FLAG(pFds[i].events, something))
			{
				FD_SET(pFds[i].fd, &ReadSet);
			}
			if (TEST_FLAG(pFds[i].events, something))
			{
				FD_SET(pFds[i].fd, &WriteSet);
			}
			if (TEST_FLAG(pFds[i].events, something))
			{
				FD_SET(pFds[i].fd, &ExceptionSet);
			}
			pFds[i].revents = 0;
		}

		s32 nRes = Select(nLargestFD, &ReadSet, &WriteSet, &ExceptionSet, (nTimeout >= 0) ? &tv : NULL);
		if (nRes > 0)
		{
			nRes = 0;
			for (u32 i = 0; i < nFds; ++i)
			{	
				if (FD_ISSET(pFds[i].fd, &ReadSet))
				{
					SET_FLAG(pFds[i].revents, something);
				}
				if (FD_ISSET(pFds[i].fd, &WriteSet))
				{
					SET_FLAG(pFds[i].revents, something);
				}
				if (FD_ISSET(pFds[i].fd, &ExceptionSet))
				{
					SET_FLAG(pFds[i].revents, something);
				}
				if (0 != pFds[i].revents)
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
	return setsockopt(nSocket, SOL_SOCKET, SO_REUSEADDR, (const s8*)&nIsReusable, sizeof(s32));
}


//----------------------------------------------------------//
//
//----------------------------------------------------------//
//-- Description
//----------------------------------------------------------//
u16 SysSocket::Ntohs(u16 nValue)
{
	return ntohs(nValue);
}


//----------------------------------------------------------//
//
//----------------------------------------------------------//
//-- Description
//----------------------------------------------------------//
u16 SysSocket::Htons(u16 nValue)
{
	return htons(nValue);
}


//----------------------------------------------------------//
//
//----------------------------------------------------------//
//-- Description
//----------------------------------------------------------//
u32 SysSocket::Ntohl(u32 nValue)
{
	return ntohl(nValue);
}


//----------------------------------------------------------//
//
//----------------------------------------------------------//
//-- Description
//----------------------------------------------------------//
u32 SysSocket::Htonl(u32 nValue)
{
	return htonl(nValue);
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
		return strDest;
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
