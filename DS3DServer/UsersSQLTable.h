#ifndef _USERS_SQLTABLE_H_
#define _USERS_SQLTABLE_H_
#pragma once

//----------------------------------------------------------//
// USERSSQLTABLE.H
//----------------------------------------------------------//
//-- Description
// Wrapper around Users SQL table access.
//----------------------------------------------------------//


#include "SQLiteTable.h"
#include "SQLiteRecord.h"
#include "Types.h"
#include "FixedString.h"


//----------------------------------------------------------//
// DEFINES
//----------------------------------------------------------//


#define USERS_EMAIL_MAX_SIZE		(64)
#define USERS_LASTIP_MAX_SIZE		(128)
#define USERS_LASTLOGIN_MAX_SIZE	(64)
#define USERS_LASTLOGOUT_MAX_SIZE	(64)


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


class CUserRecord : public CSQLiteRecord
{
	public:

		CUserRecord(const s8* strEvent, const s8* strPassword, const s8* strIP); 
		CUserRecord();
		virtual ~CUserRecord();

		static const Meta& GetMetaData(void)
		{
			return sm_MetaData;
		}

		const s8*								GetEmail(void) const;
		SysString::Hash							GetPassword(void) const;
		const s8*								GetLastLoginIP(void) const;
		const s8*								GetLastLoginTime(void) const;
		const s8*								GetLastLogoutTime(void) const;
		bool									GetIsLoggedIn(void) const;

		static s32								AccessEmail(void* pRow, Meta::ColumnInfo::AccessMode::Enum eMode, void* pData, size_t& nDataSize);
		static s32								AccessPassword(void* pRow, Meta::ColumnInfo::AccessMode::Enum eMode, void* pData, size_t& nDataSize);
		static s32								AccessLastLoginIP(void* pRow, Meta::ColumnInfo::AccessMode::Enum eMode, void* pData, size_t& nDataSize);
		static s32								AccessLastLoginTime(void* pRow, Meta::ColumnInfo::AccessMode::Enum eMode, void* pData, size_t& nDataSize);
		static s32								AccessLastLogoutTime(void* pRow, Meta::ColumnInfo::AccessMode::Enum eMode, void* pData, size_t& nDataSize);
		static s32								AccessIsLoggedIn(void* pRow, Meta::ColumnInfo::AccessMode::Enum eMode, void* pData, size_t& nDataSize);

	private:

		FixedString<USERS_EMAIL_MAX_SIZE>		m_strEmail;
		FixedString<USERS_LASTIP_MAX_SIZE>		m_strLastLoginIP;
		FixedString<USERS_LASTLOGIN_MAX_SIZE>	m_strLastLoginTime;
		FixedString<USERS_LASTLOGOUT_MAX_SIZE>	m_strLastLogoutTime;
		SysString::Hash							m_nPwd;
		bool									m_bIsLoggedIn;

		static const Meta						sm_MetaData;
		static const Meta::ColumnInfo			sm_ColumnArray[];
};

		
class CUsersSQLTable : public CSQLiteTable
{
	public:

		struct Error
		{
			enum Enum
			{
				QueryError				= 0x80000000,
				SanityFail				= 0x80000001,

				NoSuchUser				= 0x80000010,
				IncorrectPassword		= 0x80000011,

				BadFail					= -1,
				
				//-- OK
				Ok						= 0,
				LoggedIn				= 1,
				NotLoggedIn				= 2
			};
		};


		CUsersSQLTable(CSQLiteDatabase& database);
		~CUsersSQLTable();

		Error::Enum		Create(void);
		Error::Enum		Clean(void);
		Error::Enum		Add(CUserRecord& inUser);
		Error::Enum		Login(CUserRecord& inUser);
		Error::Enum		Logout(CUserRecord& inUser);
		Error::Enum		IsLoggedIn(CUserRecord& inUser) const;

	private:
};


//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//

#endif //_EVENTLOG_SQLTABLE_H_