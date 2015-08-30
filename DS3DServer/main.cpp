#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>

#include "SysSocket.h"
#include "FixedString.h"
#include "SimpleBuffer.h"
#include "SQLiteDatabase.h"
#include "SQLiteTable.h"
#include "SQLiteRecord.h"
#include "SQLiteStatement.h"

#include "Message.h"
#include "MsgMotd.h"
#include "Packet.h"
#include "PacketSerializer.h"
#include <list>


#define PORT 		"5555"
#define BACKLOG 	10


static bool bOnce = false;

void HandleReadActivity(int newsock, fd_set *set)
{
	// TODO
	FD_CLR(newsock, set);
}

void HandleWriteActivity(int newsock, fd_set *set)
{
	// TODO
	if (bOnce == true)
	{
		FD_CLR(newsock, set);
		return;
	}

	CMsgMotd* pMotd = new CMsgMotd();
	pMotd->SetText("This is a test.");

	TMessageList sendList;

	sendList.push_back(SysSmartPtr<CMessage>(pMotd));
	TMessageList::iterator it;
	bool bByeDetected;

	CPacket packet(SysString::GenerateKey(0,0), CPacket::Flag::Encrypted);
	CPacket::Error::Enum ePacketEC = packet.AddMessages(sendList, it, bByeDetected);
	if (ePacketEC != CPacket::Error::Ok)
	{
		printf("packet.AddMessage ePacketEC = %d\n", ePacketEC);
		fflush(stdout);
	}

	CSimpleBuffer<1024> buffer;

	CPacketSerializer ser(CSerializer::Mode::Serializing, buffer.Buffer(), buffer.Size());

	ePacketEC = packet.Serialize(ser);
	if (ePacketEC != CPacket::Error::Ok)
	{
		printf("packet.Serialize ePacketEC = %d\n", ePacketEC);
		fflush(stdout);
	}

	printf("packet has %d messages\n", packet.GetMessageCount());
	printf("packet has %d bytes\n", packet.GetDataSize());
	fflush(stdout);

	buffer.InsTail(NULL, ser.GetOffset());

	printf("buffer has %d bytes\n", buffer.UsedSize());
	fflush(stdout);

	s32 nBytes = SysSocket::Send(newsock, (const s8*)buffer.ConstBuffer(), buffer.UsedSize());
	if (nBytes > 0)
	{
		buffer.StripHead(nBytes);

		printf("send(%d)\n", nBytes);
		fflush(stdout);
	}

	FD_CLR(newsock, set);
	bOnce = true;
}


class RowClass : public CSQLiteRecord
{
	public:

		RowClass(const s8* strEvent, const s8* strDetails, const s8* strDatetime) 
		: CSQLiteRecord()
		{
			m_event = strEvent;
			m_details = strDetails;
			m_datetime = strDatetime;
		}
		RowClass()
		: CSQLiteRecord()
		{
			m_event.Clear();
			m_details.Clear();
			m_datetime.Clear();
		}
		virtual ~RowClass() 
		{
		}

		static const Meta& GetMetaData(void)
		{
			return sm_MetaData;
		}

		static s32 AccessEvent(void* pRow, Meta::ColumnInfo::AccessMode::Enum eMode, void* pData, size_t& nDataSize)
		{
			if ( IS_NULL_PTR(pRow) 
				|| IS_NULL_PTR(pData) )
			{
				return SQLITE_ERROR;
			}
			
			RowClass* pThis = (RowClass*)pRow;
			s8** ppText = (s8**)pData;

			if (Meta::ColumnInfo::AccessMode::Get == eMode)
			{
				*ppText = pThis->m_event.Buffer(); 
				nDataSize = pThis->m_event.Size();
			}
			else
			{
				pThis->m_event = *ppText;
			}

			return SQLITE_OK;
		}

		static s32 AccessDetails(void* pRow, Meta::ColumnInfo::AccessMode::Enum eMode, void* pData, size_t& nDataSize)
		{
			if ( IS_NULL_PTR(pRow) 
				|| IS_NULL_PTR(pData) )
			{
				return SQLITE_ERROR;
			}
			
			RowClass* pThis = (RowClass*)pRow;
			s8** ppText = (s8**)pData;

			if (Meta::ColumnInfo::AccessMode::Get == eMode)
			{
				*ppText = pThis->m_details.Buffer(); 
				nDataSize = pThis->m_details.Size();
			}
			else
			{
				pThis->m_details = *ppText;
			}

			return SQLITE_OK;
		}

		static s32 AccessDatetime(void* pRow, Meta::ColumnInfo::AccessMode::Enum eMode, void* pData, size_t& nDataSize)
		{
			if ( IS_NULL_PTR(pRow) 
				|| IS_NULL_PTR(pData) )
			{
				return SQLITE_ERROR;
			}
			
			RowClass* pThis = (RowClass*)pRow;
			s8** ppText = (s8**)pData;

			if (Meta::ColumnInfo::AccessMode::Get == eMode)
			{
				*ppText = pThis->m_datetime.Buffer(); 
				nDataSize = pThis->m_datetime.Size();
			}
			else
			{
				pThis->m_datetime = *ppText;
			}

			return SQLITE_OK;
		}

		FixedString<64>		m_event;
		FixedString<256>	m_details;
		FixedString<32>		m_datetime;

	private:

		static const Meta					sm_MetaData;
		static const Meta::ColumnInfo		sm_ColumnArray[];
};

const RowClass::Meta::ColumnInfo RowClass::sm_ColumnArray[] =
{
	{ "event",		Meta::ColumnInfo::Type::Text,		AccessEvent		},
	{ "details",	Meta::ColumnInfo::Type::Text,		AccessDetails	},
	{ "logTime",	Meta::ColumnInfo::Type::Text,		AccessDatetime	},
	{ "",			Meta::ColumnInfo::Type::End,		NULL			}
};
const RowClass::Meta RowClass::sm_MetaData =
{
	RowClass::sm_ColumnArray
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

	CSQLiteTable table(database, "EventLog");

	{
		//-- Test the statement builder.
		RowClass trow("wiggleBB", "BB extra", "one");
		CSQLiteStatement<RowClass> stmt(database);
			
		stmt.Prepare("INSERT INTO EventLog (event, details, logTime) VALUES (@event, @details, @logTime);");
		stmt.Bind(&trow);

		SysSmartPtr<CSQLiteStatement<RowClass>::ExecResult> execResult2 = stmt.Exec();
		
		for (CSQLiteStatement<RowClass>::ExecResult::TRows::iterator it = execResult2->m_Results.begin(); it != execResult2->m_Results.end(); ++it)
		{
			printf("exec2: %s, %s, %s\n", it->m_event.ConstBuffer(), it->m_details.ConstBuffer(), it->m_datetime.ConstBuffer());
		}

		printf("count exec2: %d\n", execResult2.GetRefCount());
	}

	{
		//-- Test the table. Exec method for batch 
		SysSmartPtr<CSQLiteTable::ExecResult> execResult = table.Exec("SELECT * FROM EventLog;");
		for (CSQLiteTable::ExecResult::TRows::iterator it = execResult->m_Rows.begin(); it != execResult->m_Rows.end(); ++it)
		{
			for (CSQLiteTable::ExecResult::TValues::iterator it2 = it->begin(); it2 != it->end(); ++it2)
			{
				printf("| %s ", it2->c_str());
			}
			printf("\n");
		}

		printf("count exec: %d\n", execResult.GetRefCount());
	}

	int maxsock;
	SysSocket::Socket sock;
	SysSocket::FdSet socks;
	SysSocket::FdSet readsocks, writesocks;
	SysSocket::AddrInfo hints, *res;


	//-- Get address info
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	if (SYS_SOCKET_NO_ERROR != SysSocket::GetInfo(NULL, PORT, &hints, &res))
	{
		perror("getaddrinfo");
		return 1;
	}

	//-- Create listening socket
	sock = SysSocket::OpenSocket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (SysSocket::INVALID_SOCK == sock)
	{
		perror("socket");
		return 2;
	}

	//-- Enable reusable address
	if (SYS_SOCKET_NO_ERROR != SysSocket::SetReusable(sock, true))
	{
		perror("setsockopt");
		return 3;
	}

	//-- Bind
	if (SYS_SOCKET_NO_ERROR != SysSocket::Bind(sock, res->ai_addr, res->ai_addrlen))
	{
		perror("bind");
		return 35;
	}

	SysSocket::FreeInfo(res);

	//-- listen
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
		writesocks = socks;

		if (SYS_SOCKET_ERROR == SysSocket::Select(maxsock+1, &readsocks, &writesocks, NULL, NULL))
		{
			perror("select");
			return 5;
		}

		for (int s = 0; s <= maxsock; ++s)
		{
			if (FD_ISSET(s, &writesocks))
			{
				//printf("WRITE activity on socket %d\n", s);
				//fflush(stdout);

				HandleWriteActivity(s, &writesocks);
			}

			if (FD_ISSET(s, &readsocks))
			{
//				printf("READ Activity on socket %d\n", s);
//				fflush(stdout);
				
				if (s == sock)
				{
					//-- listening socket activity
					SysSocket::Socket newsock;
					SysSocket::SockAddrIn other_addr;
					size_t nSize = sizeof(other_addr);
					newsock = SysSocket::Accept(sock, (SysSocket::SockAddr*)&other_addr, &nSize);
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

						SysSmartPtr<CSQLiteTable::ExecResult> insertResult = table.Exec(strExec.ConstBuffer());
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
					HandleReadActivity(s, &readsocks);
				}
			}
		}
	}

	rc = database.Close();

	SysSocket::CloseSocket(sock);

	return 0;
}
