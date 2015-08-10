//----------------------------------------------------------//
// SQLITETABLE.CPP
//----------------------------------------------------------//
//-- Description			
// A SQLite table wrapper.
//----------------------------------------------------------//


#include "SQLiteTable.h"
#include "SQLiteDatabase.h"
#include "Types.h"
#include "FixedString.h"
#include "SysSmartPtr.h"
#include <list>
#include <string>
#include <sqlite3.h>


//----------------------------------------------------------//
// DEFINES
//----------------------------------------------------------//

//----------------------------------------------------------//
// GLOBALS
//----------------------------------------------------------//

//----------------------------------------------------------//
// CSQLiteTable::ExecResult::ExecResult
//----------------------------------------------------------//
//-- Description
//----------------------------------------------------------//
CSQLiteTable::ExecResult::ExecResult()
: m_nErrorCode(SQLITE_OK)
, m_pStrError(NULL)
{
	m_Rows.clear();
}


//----------------------------------------------------------//
// CSQLiteTable::ExecResult::~ExecResult
//----------------------------------------------------------//
//-- Description
//----------------------------------------------------------//
CSQLiteTable::ExecResult::~ExecResult()
{
	m_Rows.clear();
	sqlite3_free(m_pStrError);
}


//----------------------------------------------------------//
// CSQLiteTable::CSQLiteTable
//----------------------------------------------------------//
//-- Description
//----------------------------------------------------------//
CSQLiteTable::CSQLiteTable(CSQLiteDatabase& database, const s8* strTableName)
: m_Database(database)
{
	m_strTableName.Set(strTableName);
}
		

//----------------------------------------------------------//
// CSQLiteTable::~CSQLiteTable
//----------------------------------------------------------//
//-- Description
//----------------------------------------------------------//
CSQLiteTable::~CSQLiteTable()
{
}


//----------------------------------------------------------//
// CSQLiteTable::Exec
//----------------------------------------------------------//
//-- Description
// Batch processing of non-prepared, non-parameter command sets.
// Pros: 
//   Can contain multiple ;-separated commands.
//   May return an error string in result.
// Cons: 
//   Will only return result as a set of strings.
//   Cannot use parameters in command string.
//   Will not bind or retrieve values into templated row class.
//----------------------------------------------------------//
SysSmartPtr<CSQLiteTable::ExecResult> CSQLiteTable::Exec(const s8* strCommands)
{
	ExecResult* pExecResult = new ExecResult();
	if (IS_PTR(pExecResult))
	{		
		s32 nErrorCode = sqlite3_exec(m_Database.GetHandle(), strCommands, ExecCallback, pExecResult, &pExecResult->m_pStrError);
		if (SQLITE_OK == pExecResult->m_nErrorCode)
		{
			pExecResult->m_nErrorCode = nErrorCode;
		}
	}

	return SysSmartPtr<ExecResult>(pExecResult);
}


//----------------------------------------------------------//
// CSQLiteTable::ExecCallback
//----------------------------------------------------------//
//-- Description
// Static callback function required to process row results
// from sqlite3_exec().
//----------------------------------------------------------//
s32 CSQLiteTable::ExecCallback(void* pResult, s32 nValues, s8** ppValues, s8** ppNames)
{
	if ( IS_PTR(pResult)
		&& IS_NOT_ZERO(nValues) )
	{
		ExecResult* pExecResult = (ExecResult*)pResult;
		ExecResult::TValues row;

		for (s32 i = 0; i < nValues; ++i)
		{
			row.push_back(ppValues[i] ? ppValues[i] : "null");
		}

		pExecResult->m_Rows.push_back(row);
	}

	return 0;
}


//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//
