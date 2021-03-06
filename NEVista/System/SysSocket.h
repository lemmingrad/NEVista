#ifndef _SYS_SOCKET_H_
#define _SYS_SOCKET_H_

//----------------------------------------------------------//
// SYSSOCKET.H
//----------------------------------------------------------//
//-- Description
// Wrapper for socket functions to hide implementation of 
// a socket on a platform.
//----------------------------------------------------------//


#if defined(WIN32)

#	define SYSSOCKET_USES_WINSOCK
#	include <WS2tcpip.h>
#	include <winsock2.h>

#elif defined(LINUX)

#	define SYSSOCKET_USES_BSD
#	include <errno.h>
#	include <poll.h>
#	include <sys/types.h>
#	include <sys/socket.h>
#	include <arpa/inet.h>
#	include <netinet/in.h>
#	include <netdb.h>

#endif

#include "Types.h"
#include <vector>


//----------------------------------------------------------//
// DEFINES
//----------------------------------------------------------//


#define SYS_SOCKET_NO_ERROR				(0)
#define SYS_SOCKET_RECV_ZERO			(-2)
#define SYS_SOCKET_WOULD_BLOCK			(-3)
#define SYS_SOCKET_IN_PROGRESS			(-4)

#if defined(SYSSOCKET_USES_WINSOCK)

#	define SYS_SOCKET_ERROR				(SOCKET_ERROR)

#else

#	define SYS_SOCKET_ERROR				(-1)

#endif //SYSSOCKET_USES_WINSOCK

#define SYSSOCKET_IPADDRESS_MAX_SIZE	(INET6_ADDRSTRLEN)


//----------------------------------------------------------//
// ENUMS
//----------------------------------------------------------//

//----------------------------------------------------------//
// STRUCTS
//----------------------------------------------------------//

//----------------------------------------------------------//
// CLASSES
//----------------------------------------------------------//


class SysSocket
{
	public:

#if defined(SYSSOCKET_USES_WINSOCK)
		typedef SOCKET						Socket;
#else
		typedef int							Socket;
#endif //SYSSOCKET_USES_WINSOCK

		static const Socket					INVALID_SOCK;

		typedef struct sockaddr				SockAddr;
		typedef struct sockaddr_in			SockAddrIn;
		typedef struct in_addr				InAddr;
		typedef struct sockaddr_in6			SockAddrIn6;
		typedef struct in6_addr				In6Addr;
		typedef struct sockaddr_storage		SockAddrStorage;
		typedef struct addrinfo				AddrInfo;
		typedef struct timeval				Timeval;
		typedef fd_set						FdSet;
#if defined(SYSSOCKET_USES_WINSOCK)
		struct SysSocketPollFd
		{
			Socket		fd;				/* file descriptor */
			short		events;			/* requested events */
			short		revents;		/* returned events */
		};
		typedef SysSocketPollFd				PollFd;
		struct Shutdown
		{
			enum Enum
			{
				Read = SD_RECEIVE,
				Write = SD_SEND,
				Both = SD_BOTH 
			};
		};
#else
		typedef struct pollfd				PollFd;
		struct Shutdown
		{
			enum Enum
			{
				Read = SHUT_RD,
				Write = SHUT_WR,
				Both = SHUT_RDWR 
			};
		};
#endif //SYSSOCKET_USES_WINSOCK
		typedef std::vector<PollFd>			PollFdSet;

		class Address
		{
			public:
				Address(const SockAddr* pAddr);
				Address();
				~Address();

				SysSocket::SockAddr*			GetSockAddr(void);
				size_t*							GetSockAddrSize(void);
				const s8*						GetIPAddress(s8* strBuffer, size_t nStrBufferSize);
				u16								GetPort(void);
			private:
				SysSocket::SockAddrStorage		m_addr;
				size_t							m_nAddrSize;
		};

		static void			SystemInitialise(void);
		static void			SystemShutdown(void);

		static s32			GetInfo(const s8* strName, const s8* strPort, const AddrInfo* pHints, AddrInfo** pResults);
		static void			FreeInfo(AddrInfo* pResults);

		static Socket		OpenSocket(s32 nAF, s32 nType, s32 nProtocol);
		static s32			Bind(Socket nSocket, const SockAddr* pAddress, size_t nAddressSize);

		static s32			Connect(Socket nSocket, const SockAddr* pAddress, size_t nAddressSize);
		static s32			Listen(Socket nSocket, s32 nBacklog);
		static Socket		Accept(Socket nSocket, SockAddr* pAddress, size_t* pAddressSize);

		static s32			Select(s32 nLargestFD, FdSet* pReadFDs, FdSet* pWriteFDs, FdSet* pExceptFDs, struct timeval* pTimeout);
		static s32			Poll(PollFd* pFds, u32 nFds, s32 nTimeout);

		static s32			Shutdown(Socket nSocket, s32 nHow);
		static s32			CloseSocket(Socket nSocket);

		static s32			SetNonblocking(Socket nSocket, bool bIsNonblocking);
		static s32			SetReusable(Socket nSocket, bool bIsReusable);
		static s32			SetSockOpt(Socket nSocket, s32 nLevel, s32 nOption, const void* pOptVal, size_t nOptValSize);
		static s32			GetSockOpt(Socket nSocket, s32 nLevel, s32 nOption, void* pOptVal, size_t* pOptValSize);

		static u16			Ntohs(u16 nValue);
		static u16			Htons(u16 nValue);
		static u32			Ntohl(u32 nValue);
		static u32			Htonl(u32 nValue);
		static s32			InetPToN(s32 nType, const s8* strSrc, void* pDest, size_t nDestSize);
		static const s8*	InetNToP(s32 nType, const void* pSrc, size_t nSrcSize, s8* strDest, size_t nDestSize);

		static s32			Send(Socket nSocket, const s8* pBuffer, size_t nBufferSize);
		static s32			Sendto(Socket nSocket, const s8* pBuffer, size_t nBufferSize, const SockAddr* pAddr, size_t nAddrSize);
		static s32			Recv(Socket nSocket, s8* pBuffer, size_t nBufferSize);
		static s32			Recvfrom(Socket nSocket, s8* pBuffer, size_t nBufferSize, SockAddr* pAddr, size_t* pAddrSize);

	private:
			
		SysSocket();
		~SysSocket();
};


//----------------------------------------------------------//
// EXTERNALS
//----------------------------------------------------------//

//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//

#endif
