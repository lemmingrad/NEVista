//----------------------------------------------------------//
// EVENTLOGSQLTABLE.CPP
//----------------------------------------------------------//
//-- Description
// Wrapper around EventLog SQL table access.
//----------------------------------------------------------//


#include "EventLogSQLTable.h"
#include "Types.h"
#include "FixedString.h"
#include "SQLiteDatabase.h"
#include "SQLiteTable.h"
#include "SQLiteStatement.h"
#include <sqlite3.h>


//----------------------------------------------------------//
// DEFINES
//----------------------------------------------------------//

//----------------------------------------------------------//
// GLOBALS
//----------------------------------------------------------//


CEventLogEvent::CEventLogEvent(const s8* strEvent, const s8* strDetails)
: CSQLiteRecord()
{
	m_strEvent = strEvent;
	m_strDetails = strDetails;
	m_strDatetime.Clear();
}

CEventLogEvent::CEventLogEvent()
: CSQLiteRecord()
{
	m_strEvent.Clear();
	m_strDetails.Clear();
	m_strDatetime.Clear();
}


CEventLogEvent::~CEventLogEvent()
{
}


const s8* CEventLogEvent::GetEvent(void) const
{
	return m_strEvent.ConstBuffer();
}


const s8* CEventLogEvent::GetDetails(void) const
{
	return m_strDetails.ConstBuffer();
}


const s8* CEventLogEvent::GetDatetime(void) const
{
	return m_strDatetime.ConstBuffer();
}


s32 CEventLogEvent::AccessEvent(void* pRow, Meta::ColumnInfo::AccessMode::Enum eMode, void* pData, size_t& nDataSize)
{
	if ( IS_NULL_PTR(pRow) 
		|| IS_NULL_PTR(pData) )
	{
		return SQLITE_ERROR;
	}
	
	CEventLogEvent* _this = (CEventLogEvent*)pRow;
	s8** ppText = (s8**)pData;

	if (Meta::ColumnInfo::AccessMode::Get == eMode)
	{
		*ppText = _this->m_strEvent.Buffer();
		nDataSize = _this->m_strEvent.Length() + 1;
	}
	else
	{
		_this->m_strEvent = *ppText;
	}

	return SQLITE_OK;
}


s32	CEventLogEvent::AccessDetails(void* pRow, Meta::ColumnInfo::AccessMode::Enum eMode, void* pData, size_t& nDataSize)
{
	if ( IS_NULL_PTR(pRow) 
		|| IS_NULL_PTR(pData) )
	{
		return SQLITE_ERROR;
	}
	
	CEventLogEvent* _this = (CEventLogEvent*)pRow;
	s8** ppText = (s8**)pData;

	if (Meta::ColumnInfo::AccessMode::Get == eMode)
	{
		*ppText = _this->m_strDetails.Buffer();
		nDataSize = _this->m_strDetails.Length() + 1;
	}
	else
	{
		_this->m_strDetails = *ppText;
	}

	return SQLITE_OK;
}


s32 CEventLogEvent::AccessDatetime(void* pRow, Meta::ColumnInfo::AccessMode::Enum eMode, void* pData, size_t& nDataSize)
{
	if ( IS_NULL_PTR(pRow) 
		|| IS_NULL_PTR(pData) )
	{
		return SQLITE_ERROR;
	}
	
	CEventLogEvent* _this = (CEventLogEvent*)pRow;
	s8** ppText = (s8**)pData;

	if (Meta::ColumnInfo::AccessMode::Get == eMode)
	{
		*ppText = _this->m_strDatetime.Buffer(); 
		nDataSize = _this->m_strDatetime.Length() + 1;
	}
	else
	{
		_this->m_strDatetime = *ppText;
	}

	return SQLITE_OK;
}


const CEventLogEvent::Meta::ColumnInfo CEventLogEvent::sm_ColumnArray[] =
{
	{ "logTime",	Meta::ColumnInfo::Type::Text,		AccessDatetime	},
	{ "event",		Meta::ColumnInfo::Type::Text,		AccessEvent		},
	{ "details",	Meta::ColumnInfo::Type::Text,		AccessDetails	},
	{ "",			Meta::ColumnInfo::Type::End,		NULL			}
};
const CEventLogEvent::Meta CEventLogEvent::sm_MetaData =
{
	CEventLogEvent::sm_ColumnArray
};


CEventLogSQLTable::CEventLogSQLTable(CSQLiteDatabase& database)
: CSQLiteTable(database, "EventLog")
{
}


CEventLogSQLTable::~CEventLogSQLTable()
{
}


CEventLogSQLTable::Error::Enum CEventLogSQLTable::Create(void)
{
	SysSmartPtr<ExecResult>	pResult = Exec("CREATE TABLE EventLog(logTime TEXT, event TEXT, details TEXT);");
	if (SQLITE_OK != pResult->m_nErrorCode)
	{
		return Error::QueryError;
	}

	return Error::Ok;
}


CEventLogSQLTable::Error::Enum CEventLogSQLTable::Clean(void)
{
	SysSmartPtr<ExecResult>	pResult = Exec("DELETE FROM EventLog;");
	if (SQLITE_OK != pResult->m_nErrorCode)
	{
		return Error::QueryError;
	}

	return Error::Ok;
}


CEventLogSQLTable::Error::Enum CEventLogSQLTable::AddEvent(CEventLogEvent& inEvent)
{
	CSQLiteStatement<CEventLogEvent> stmt(m_database);

	stmt.Prepare("INSERT INTO EventLog (logTime, event, details) VALUES (datetime('now'), @event, @details);");
	stmt.Bind(&inEvent);
	
	SysSmartPtr<CSQLiteStatement<CEventLogEvent>::ExecResult> pResult = stmt.Exec();
	if (SQLITE_OK != pResult->m_nErrorCode)
	{
		return Error::QueryError;
	}

	return Error::Ok;
}


//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//
