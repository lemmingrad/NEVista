#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>

#include "SysSocket.h"
#include "FixedString.h"
#include "SQLiteDatabase.h"
#include "SQLiteTable.h"
#include "SQLiteRecord.h"


#define PORT 		"5555"
#define BACKLOG 	10


void HandleActivity(int newsock, fd_set *set)
{
	// TODO
	FD_CLR(newsock, set);
}


class RowClass : public SQLiteRecord
{
	public:
		RowClass(const s8* strEvent, const s8* strDetails, const s8* strDatetime) 
		: SQLiteRecord()
		{
			m_event = strEvent;
			m_details = strDetails;
			m_datetime = strDatetime;
		}
		RowClass()
		: SQLiteRecord()
		{
			m_event.Clear();
			m_details.Clear();
			m_datetime.Clear();
		}
		virtual ~RowClass() 
		{
		}

		static s32 GetEvent(void* pRow, void* pData, size_t& nDataSize)
		{
			RowClass* pThis = (RowClass*)pRow;
			s8** pText = (s8**)pData;
			*pText = pThis->m_event.Buffer(); 
			return SQLITE_OK;
		}
		static s32 SetEvent(void* pRow, void* pData, size_t nDataSize)
		{
			RowClass* pThis = (RowClass*)pRow;
			s8* pText = (s8*)pData;
			pThis->m_event = pText;
			return SQLITE_OK;
		}

		static s32 GetDetails(void* pRow, void* pData, size_t& nDataSize)
		{
			RowClass* pThis = (RowClass*)pRow;
			s8** pText = (s8**)pData;
			*pText = pThis->m_details.Buffer(); 
			return SQLITE_OK;
		}
		static s32 SetDetails(void* pRow, void* pData, size_t nDataSize)
		{
			RowClass* pThis = (RowClass*)pRow;
			s8* pText = (s8*)pData;
			pThis->m_details = pText;
			return SQLITE_OK;
		}

		static s32 GetDatetime(void* pRow, void* pData, size_t& nDataSize)
		{
			RowClass* pThis = (RowClass*)pRow;
			s8** pText = (s8**)pData;
			*pText = pThis->m_datetime.Buffer(); 
			return SQLITE_OK;
		}
		static s32 SetDatetime(void* pRow, void* pData, size_t nDataSize)
		{
			RowClass* pThis = (RowClass*)pRow;
			s8* pText = (s8*)pData;
			pThis->m_datetime = pText;
			return SQLITE_OK;
		}

		static s32 ExecCallback(void* pResult, s32 argc, s8** argv, s8** pCols)
		{
			printf("ExecCallback:\n");
			fflush(stdout);

			CSQLiteTable<RowClass>::ExecResult* pExecResult = (CSQLiteTable<RowClass>::ExecResult*)pResult;
			if (IS_PTR(pExecResult))
			{
				pExecResult->m_Results.push_back(RowClass());
				RowClass& row = pExecResult->m_Results.back();
				
				for (s32 i = 0; i < argc; ++i)
				{
					printf("New: %s=%s\n", pCols[i], argv[i] ? argv[i] : "null");
					fflush(stdout);

					for (u32 ic = 0; ic < sm_nInfoCount; ++ic)
					{
						if (IS_ZERO(SysString::Strcmp(pCols[i], sm_InfoList[ic].m_strName.ConstBuffer())))
						{
							sm_InfoList[ic].m_pSetFunc(&row, argv[i] ? (void*)argv[i] : (void*)"null", 0);
							break;
						}
					}
				}
			}

			return 0;
		}

		FixedString<64>		m_event;
		FixedString<256>	m_details;
		FixedString<32>		m_datetime;

		static Info			sm_InfoList[];
		static u32			sm_nInfoCount;

	private:
};

u32 RowClass::sm_nInfoCount = 3;
RowClass::Info RowClass::sm_InfoList[] = 
{
	{ "event",		RowClass::Type::Text,	RowClass::GetEvent,			RowClass::SetEvent		},
	{ "details",	RowClass::Type::Text,	RowClass::GetDetails,		RowClass::SetDetails	},
	{ "logTime",	RowClass::Type::Text,	RowClass::GetDatetime,		RowClass::SetDatetime	}
};








int main(int argc, char** argv)
{

	CSQLiteDatabase database;
	
	s32 rc = database.Open("Data/test.db");
	if (rc)
	{
		perror("Didn't open database.");
		database.Close();
		return 50;
	}

	CSQLiteTable<RowClass> table(database, "EventLog");

	{
		SysSmartPtr<CSQLiteTable<RowClass>::Statement> stmt = table.NewStatement();
		stmt->Prepare("INSERT INTO EventLog (event, details, logTime) VALUES (@event, @details, @logTime);");
		RowClass trow("wiggle", "a wiggle", "zero");
		stmt->Bind(&trow);
		SysSmartPtr<CSQLiteTable<RowClass>::ExecResult> execResult2 = stmt->Exec();
		for (CSQLiteTable<RowClass>::ExecResult::TRows::iterator it = execResult2->m_Results.begin(); it != execResult2->m_Results.end(); ++it)
		{
			printf("New3: %s, %s, %s\n", it->m_event.ConstBuffer(), it->m_details.ConstBuffer(), it->m_datetime.ConstBuffer());
		}

		printf("count exec2: %d\n", execResult2.GetRefCount());
		printf("count stmt: %d\n", stmt.Count());
	}

	{
		SysSmartPtr<CSQLiteTable<RowClass>::ExecResult> execResult = table.Exec("SELECT * FROM EventLog;");
		for (CSQLiteTable<RowClass>::ExecResult::TRows::iterator it = execResult->m_Results.begin(); it != execResult->m_Results.end(); ++it)
		{
			printf("New2: %s, %s, %s\n", it->m_event.ConstBuffer(), it->m_details.ConstBuffer(), it->m_datetime.ConstBuffer());
		}

		printf("count exec: %d\n", execResult.Count());
	}

//	int sock;
//	fd_set socks;
//	fd_set readsocks;
	int maxsock;
//	int reuseaddr = 1;
//	struct addrinfo hints, *res;

	SysSocket::Socket sock;
	SysSocket::FdSet socks;
	SysSocket::FdSet readsocks;
	SysSocket::AddrInfo hints, *res;


	//-- Get address info
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
//	if (0 != getaddrinfo(NULL, PORT, &hints, &res))
	if (SYS_SOCKET_NO_ERROR != SysSocket::GetInfo(NULL, PORT, &hints, &res))
	{
		perror("getaddrinfo");
		return 1;
	}

	//-- Create listening socket
//	sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	sock = SysSocket::OpenSocket(res->ai_family, res->ai_socktype, res->ai_protocol);
//	if (sock == -1)
	if (SysSocket::INVALID_SOCK == sock)
	{
		perror("socket");
		return 2;
	}

	//-- Enable reusable address
//	if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &reuseaddr, sizeof(int)) == -1)
	if (SYS_SOCKET_NO_ERROR != SysSocket::SetReusable(sock, true))
	{
		perror("setsockopt");
		return 3;
	}

	//-- Bind
//	if (bind(sock, res->ai_addr, res->ai_addrlen) == -1)
	if (SYS_SOCKET_NO_ERROR != SysSocket::Bind(sock, res->ai_addr, res->ai_addrlen))
	{
		perror("bind");
		return 35;
	}

//	freeaddrinfo(res);
	SysSocket::FreeInfo(res);

	//-- listen
//	if (listen(sock, BACKLOG) == -1)
	if (SYS_SOCKET_ERROR == SysSocket::Listen(sock, BACKLOG))
	{
		perror("listen");
		return 4;
	}
	else
	{
		printf("Listening on port "PORT"\n");
		fflush(stdout);
	}

	//-- clear fd set
	FD_ZERO(&socks);
	FD_SET(sock, &socks);
	maxsock = sock;


	while (1)
	{
		readsocks = socks;

//		if (select(maxsock+1, &readsocks, NULL, NULL, NULL) == -1)
		if (SYS_SOCKET_ERROR == SysSocket::Select(maxsock+1, &readsocks, NULL, NULL, NULL))
		{
			perror("select");
			return 5;
		}

		for (int s = 0; s <= maxsock; ++s)
		{
			if (FD_ISSET(s, &readsocks))
			{
				printf("Activity on socket %d\n", s);
				fflush(stdout);
				
				if (s == sock)
				{
					//-- listening socket activity
//					int newsock;
//					struct sockaddr_in other_addr;
//					size_t nSize = sizeof(struct sockaddr_in);
//					newsock = accept(sock, (struct sockaddr*)&other_addr, &nSize);
					SysSocket::Socket newsock;
					SysSocket::SockAddrIn other_addr;
					size_t nSize = sizeof(other_addr);
					newsock = SysSocket::Accept(sock, (SysSocket::SockAddr*)&other_addr, &nSize);
//					if (newsock == -1) 
					if (SysSocket::INVALID_SOCK == newsock)
					{
						perror("accept");
					}
					else
					{
						FixedString<INET_ADDRSTRLEN> strIP;
						if (IS_NULL_PTR(SysSocket::InetNToP(AF_INET, &other_addr.sin_addr, sizeof(other_addr.sin_addr), strIP.Buffer(), strIP.Size())))
						{
							//-- error
							perror("inet_ntop");
						}

						printf("New connection from %s on port %d\n",
							strIP.ConstBuffer(), SysSocket::Htons(other_addr.sin_port));

						fflush(stdout);

						FixedString<256> strExec;

						strExec = "INSERT INTO EventLog VALUES(datetime('now'), \"Connection\", \"Connection attempt made from ";
						strExec += strIP.ConstBuffer();
						strExec += "\");";
						printf("%s\n", strExec.ConstBuffer());

						SysSmartPtr<CSQLiteTable<RowClass>::ExecResult> insertResult = table.Exec(strExec.ConstBuffer());
						if (insertResult->m_nErrorCode != SQLITE_OK)
						{
							perror("sql exec");
						}

						FD_SET(newsock, &socks);
						if ((s32)newsock > maxsock)
						{
							maxsock = (s32)newsock;
						}
					}
				}
				else
				{
					HandleActivity(s, &socks);
				}
			}
		}
	}


	rc = database.Close();

//	close(sock);
	SysSocket::CloseSocket(sock);

	return 0;
}
