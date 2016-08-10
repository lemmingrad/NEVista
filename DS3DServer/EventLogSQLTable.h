#ifndef _EVENTLOG_SQLTABLE_H_
#define _EVENTLOG_SQLTABLE_H_
#pragma once

//----------------------------------------------------------//
// EVENTLOGSQLTABLE.H
//----------------------------------------------------------//
//-- Description
// Wrapper around EventLog SQL table access.
//----------------------------------------------------------//


#include "SQLiteTable.h"
#include "SQLiteRecord.h"
#include "Types.h"
#include "FixedString.h"


//----------------------------------------------------------//
// DEFINES
//----------------------------------------------------------//


#define EVENTLOG_EVENT_MAX_SIZE		(32)
#define EVENTLOG_DETAILS_MAX_SIZE	(256)
#define EVENTLOG_DATETIME_MAX_SIZE	(64)


//----------------------------------------------------------//
// ENUMS
//----------------------------------------------------------//

//----------------------------------------------------------//
// FORWARD REFERENCES
//----------------------------------------------------------//


class CSQLiteDatabase;


//----------------------------------------------------------//
// STRUCTS
//----------------------------------------------------------//

//----------------------------------------------------------//
// CLASSES
//----------------------------------------------------------//


class CEventLogEvent : public CSQLiteRecord
{
	public:

		CEventLogEvent(const s8* strEvent, const s8* strDetails); 
		CEventLogEvent();
		virtual ~CEventLogEvent();

		static const Meta& GetMetaData(void)
		{
			return sm_MetaData;
		}

		const s8*								GetEvent(void) const;
		const s8*								GetDetails(void) const;
		const s8*								GetDatetime(void) const;

		static s32								AccessEvent(void* pRow, Meta::ColumnInfo::AccessMode::Enum eMode, void* pData, size_t& nDataSize);
		static s32								AccessDetails(void* pRow, Meta::ColumnInfo::AccessMode::Enum eMode, void* pData, size_t& nDataSize);
		static s32								AccessDatetime(void* pRow, Meta::ColumnInfo::AccessMode::Enum eMode, void* pData, size_t& nDataSize);

	private:

		FixedString<EVENTLOG_EVENT_MAX_SIZE>	m_strEvent;
		FixedString<EVENTLOG_DETAILS_MAX_SIZE>	m_strDetails;
		FixedString<EVENTLOG_DATETIME_MAX_SIZE>	m_strDatetime;

		static const Meta						sm_MetaData;
		static const Meta::ColumnInfo			sm_ColumnArray[];
};

		
class CEventLogSQLTable : public CSQLiteTable
{
	public:

		struct Error
		{
			enum Enum
			{
				QueryError				= 0x80000000,
				SanityFail				= 0x80000001,

				BadFail					= -1,
				
				//-- OK
				Ok						= 0
			};
		};

		CEventLogSQLTable(CSQLiteDatabase& database);
		~CEventLogSQLTable();

		Error::Enum		Create(void);
		Error::Enum		Clean(void);
		Error::Enum		AddEvent(CEventLogEvent& inEvent);

	private:
};


//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//

#endif //_EVENTLOG_SQLTABLE_H_