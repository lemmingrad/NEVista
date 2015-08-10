#ifndef _SQLITE_TABLE_H_
#define _SQLITE_TABLE_H_
#pragma once


//----------------------------------------------------------//
// SQLITETABLE.H
//----------------------------------------------------------//
//-- Description			
// A SQLite table wrapper.
//----------------------------------------------------------//


#include "Types.h"
#include "FixedString.h"
#include "SysSmartPtr.h"
#include <list>
#include <string>


//----------------------------------------------------------//
// DEFINES
//----------------------------------------------------------//

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


class CSQLiteTable
{
	public:
	
		struct ExecResult
		{
			typedef std::list<std::string>		TValues;
			typedef std::list<TValues>			TRows;

			ExecResult();
			~ExecResult();
			
			TRows				m_Rows;
			s32					m_nErrorCode;	
			s8*					m_pStrError;
		};
			
		CSQLiteTable(CSQLiteDatabase& database, const s8* strTableName);
		virtual ~CSQLiteTable();

		SysSmartPtr<ExecResult>	Exec(const s8* strCommands);
		static s32 ExecCallback(void* pResult, s32 nValues, s8** ppValues, s8** ppNames);

	protected:

		CSQLiteDatabase&			m_Database;
		FixedString<64>				m_strTableName;
};


//----------------------------------------------------------//
// EXTERNALS
//----------------------------------------------------------//

//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//

#endif //_SQLITE_TABLE_H_