#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "SysDebugLog.h"
#include "SysSocket.h"
#include "FixedString.h"
#include "SimpleBuffer.h"

#include <sqlite3.h>
#include "SQLiteDatabase.h"

#include "UsersSQLTable.h"

#include "Server.h"

CSQLiteDatabase gDatabase;
CServer gServer;







int main(int argc, char** argv)
{
	s32 rc = gDatabase.Open("Data/test.db");
	if (rc)
	{
		perror("Didn't open database.");
		gDatabase.Close();
		return 50;
	}
	
	CUsersSQLTable users(gDatabase);
	users.Create();
	
	CUserRecord user("a@b.com", "ABC123", "");
	users.Add(user);
	

/*
	{
		CSQLiteTable table(gDatabase, "EventLog");
	
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
*/

	CServer::Error::Enum eEr = gServer.Initialise();
	SysDebugPrintf("server init: %d\n", (int)eEr);

	while (CServer::Error::Ok == eEr)
	{
		eEr = gServer.Update();
	}
	SysDebugPrintf("server update: %d\n", (int)eEr);

	gServer.Shutdown();
	gDatabase.Close();
	return 0;
}
