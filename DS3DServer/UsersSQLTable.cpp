//----------------------------------------------------------//
// USERSSQLTABLE.CPP
//----------------------------------------------------------//
//-- Description
// Wrapper around Users SQL table access.
//----------------------------------------------------------//


#include "UsersSQLTable.h"
#include "Types.h"
#include "SysString.h"
#include "SysMemory.h"
#include "FixedString.h"
#include "SQLiteDatabase.h"
#include "SQLiteTable.h"
#include "SQLiteStatement.h"
#include <sqlite3.h>

#include "SysDebugLog.h"


//----------------------------------------------------------//
// DEFINES
//----------------------------------------------------------//

//----------------------------------------------------------//
// GLOBALS
//----------------------------------------------------------//


CUserRecord::CUserRecord(const s8* strEmail, const s8* strPassword, const s8* strIP)
: CSQLiteRecord()
{
	m_strEmail = strEmail;
	m_nPwd = SysString::GenerateHash(strPassword);
	m_strLastLoginIP = strIP;
	m_strLastLoginTime.Clear();
	m_strLastLogoutTime.Clear();
	m_bIsLoggedIn = false;
}

CUserRecord::CUserRecord()
: CSQLiteRecord()
{
	m_strEmail.Clear();
	m_nPwd = SysString::INVALID_HASH;
	m_strLastLoginIP.Clear();
	m_strLastLoginTime.Clear();
	m_strLastLogoutTime.Clear();
	m_bIsLoggedIn = false;
}


CUserRecord::~CUserRecord()
{
}


const s8* CUserRecord::GetEmail(void) const
{
	return m_strEmail.ConstBuffer();
}


SysString::Hash CUserRecord::GetPassword(void) const
{
	return m_nPwd;
}


const s8* CUserRecord::GetLastLoginIP(void) const
{
	return m_strLastLoginIP.ConstBuffer();
}


const s8* CUserRecord::GetLastLoginTime(void) const
{
	return m_strLastLoginTime.ConstBuffer();
}


const s8* CUserRecord::GetLastLogoutTime(void) const
{
	return m_strLastLogoutTime.ConstBuffer();
}


bool CUserRecord::GetIsLoggedIn(void) const
{
	return m_bIsLoggedIn;
}


s32 CUserRecord::AccessEmail(void* pRow, Meta::ColumnInfo::AccessMode::Enum eMode, void* pData, size_t& nDataSize)
{
	if ( IS_NULL_PTR(pRow) 
		|| IS_NULL_PTR(pData) )
	{
		return SQLITE_ERROR;
	}
	
	CUserRecord* _this = (CUserRecord*)pRow;
	s8** ppText = (s8**)pData;

	if (Meta::ColumnInfo::AccessMode::Get == eMode)
	{
		*ppText = _this->m_strEmail.Buffer(); 
		nDataSize = _this->m_strEmail.Length() + 1;
	}
	else
	{
		_this->m_strEmail = *ppText;
	}

	return SQLITE_OK;
}


s32	CUserRecord::AccessPassword(void* pRow, Meta::ColumnInfo::AccessMode::Enum eMode, void* pData, size_t& nDataSize)
{
	if ( IS_NULL_PTR(pRow) 
		|| IS_NULL_PTR(pData) )
	{
		return SQLITE_ERROR;
	}
	
	CUserRecord* _this = (CUserRecord*)pRow;
	s32* pnVal = (s32*)pData;

	if (Meta::ColumnInfo::AccessMode::Get == eMode)
	{
		*pnVal = _this->m_nPwd; 
		nDataSize = sizeof(s32);
	}
	else
	{
		_this->m_nPwd = *pnVal;
	}

	return SQLITE_OK;
}


s32 CUserRecord::AccessLastLoginIP(void* pRow, Meta::ColumnInfo::AccessMode::Enum eMode, void* pData, size_t& nDataSize)
{
	if ( IS_NULL_PTR(pRow) 
		|| IS_NULL_PTR(pData) )
	{
		return SQLITE_ERROR;
	}
	
	CUserRecord* _this = (CUserRecord*)pRow;
	s8** ppText = (s8**)pData;

	if (Meta::ColumnInfo::AccessMode::Get == eMode)
	{
		*ppText = _this->m_strLastLoginIP.Buffer(); 
		nDataSize = _this->m_strLastLoginIP.Length() + 1;
	}
	else
	{
		_this->m_strLastLoginIP = *ppText;
	}

	return SQLITE_OK;
}


s32 CUserRecord::AccessLastLoginTime(void* pRow, Meta::ColumnInfo::AccessMode::Enum eMode, void* pData, size_t& nDataSize)
{
	if ( IS_NULL_PTR(pRow) 
		|| IS_NULL_PTR(pData) )
	{
		return SQLITE_ERROR;
	}
	
	CUserRecord* _this = (CUserRecord*)pRow;
	s8** ppText = (s8**)pData;

	if (Meta::ColumnInfo::AccessMode::Get == eMode)
	{
		*ppText = _this->m_strLastLoginTime.Buffer(); 
		nDataSize = _this->m_strLastLoginTime.Length() + 1;
	}
	else
	{
		_this->m_strLastLoginTime = *ppText;
	}

	return SQLITE_OK;
}


s32 CUserRecord::AccessLastLogoutTime(void* pRow, Meta::ColumnInfo::AccessMode::Enum eMode, void* pData, size_t& nDataSize)
{
	if ( IS_NULL_PTR(pRow) 
		|| IS_NULL_PTR(pData) )
	{
		return SQLITE_ERROR;
	}
	
	CUserRecord* _this = (CUserRecord*)pRow;
	s8** ppText = (s8**)pData;

	if (Meta::ColumnInfo::AccessMode::Get == eMode)
	{
		*ppText = _this->m_strLastLogoutTime.Buffer(); 
		nDataSize = _this->m_strLastLogoutTime.Length() + 1;
	}
	else
	{
		_this->m_strLastLogoutTime = *ppText;
	}

	return SQLITE_OK;
}


s32 CUserRecord::AccessIsLoggedIn(void* pRow, Meta::ColumnInfo::AccessMode::Enum eMode, void* pData, size_t& nDataSize)
{
	if ( IS_NULL_PTR(pRow) 
		|| IS_NULL_PTR(pData) )
	{
		return SQLITE_ERROR;
	}
	
	CUserRecord* _this = (CUserRecord*)pRow;
	s32* pnVal = (s32*)pData;

	if (Meta::ColumnInfo::AccessMode::Get == eMode)
	{
		*pnVal = IS_TRUE(_this->m_bIsLoggedIn) ? 1 : 0; 
		nDataSize = sizeof(s32);
	}
	else
	{
		_this->m_bIsLoggedIn = IS_NOT_ZERO(*pnVal) ? true : false;
	}

	return SQLITE_OK;
}



const CUserRecord::Meta::ColumnInfo CUserRecord::sm_ColumnArray[] =
{
	{ "email",				Meta::ColumnInfo::Type::Text,		AccessEmail			},
	{ "pwd",				Meta::ColumnInfo::Type::Int,		AccessPassword		},
	{ "lastLoginIP",		Meta::ColumnInfo::Type::Text,		AccessLastLoginIP	},
	{ "lastLoginTime",		Meta::ColumnInfo::Type::Text,		AccessLastLoginTime	},
	{ "lastLogoutTime",		Meta::ColumnInfo::Type::Text,		AccessLastLogoutTime},
	{ "bIsLoggedIn",		Meta::ColumnInfo::Type::Int,		AccessIsLoggedIn	},
	{ "",					Meta::ColumnInfo::Type::End,		NULL				}
};
const CUserRecord::Meta CUserRecord::sm_MetaData =
{
	CUserRecord::sm_ColumnArray
};


CUsersSQLTable::CUsersSQLTable(CSQLiteDatabase& database)
: CSQLiteTable(database, "Users")
{
}


CUsersSQLTable::~CUsersSQLTable()
{
}


CUsersSQLTable::Error::Enum CUsersSQLTable::Create(void)
{
	SysSmartPtr<ExecResult>	pResult = Exec("CREATE TABLE Users(email TEXT PRIMARY KEY, pwd INTEGER,"
		"lastLoginIP TEXT, lastLoginTime TEXT, lastLogoutTime TEXT, bIsLoggedIn INTEGER);");
	if ( (SQLITE_OK != pResult->m_nErrorCode) 
		&& (SQLITE_DONE != pResult->m_nErrorCode) )
	{
		return Error::QueryError;
	}

	return Error::Ok;
}


CUsersSQLTable::Error::Enum CUsersSQLTable::Clean(void)
{
	SysSmartPtr<ExecResult>	pResult = Exec("DELETE FROM Users;");
	if ( (SQLITE_OK != pResult->m_nErrorCode) 
		&& (SQLITE_DONE != pResult->m_nErrorCode) )
	{
		return Error::QueryError;
	}

	return Error::Ok;
}


CUsersSQLTable::Error::Enum CUsersSQLTable::Add(CUserRecord& inUser)
{
	CSQLiteStatement<CUserRecord> ustmt(m_database);

	ustmt.Prepare("INSERT INTO Users (email, pwd, bIsLoggedIn) VALUES (@email, @pwd, 0);");
	ustmt.Bind(&inUser);
		
	SysSmartPtr<CSQLiteStatement<CUserRecord>::ExecResult> pResult = ustmt.Exec();
	if ( (SQLITE_OK != pResult->m_nErrorCode) 
		&& (SQLITE_DONE != pResult->m_nErrorCode) )
	{
		return Error::QueryError;
	}

	return Error::Ok;
}


CUsersSQLTable::Error::Enum	CUsersSQLTable::Login(CUserRecord& inUser)
{
	CSQLiteStatement<CUserRecord> qstmt(m_database);

	qstmt.Prepare("SELECT email, pwd FROM Users WHERE email = @email;");
	qstmt.Bind(&inUser);

	SysSmartPtr<CSQLiteStatement<CUserRecord>::ExecResult> pQResult = qstmt.Exec();
	if ( (SQLITE_OK != pQResult->m_nErrorCode) 
		&& (SQLITE_DONE != pQResult->m_nErrorCode) )
	{
		SysDebugPrintf("login queryerror %d\n", pQResult->m_nErrorCode);
		return Error::QueryError;
	}

	size_t nResults = pQResult->m_Results.size();

	if (IS_ZERO(nResults))
	{
		SysDebugPrintf("login nosuchuser\n");
		return Error::NoSuchUser;
	}
	if (1 != nResults)
	{
		SysDebugPrintf("login sanityfail\n");
		return Error::SanityFail;
	}

	bool bFound = false;
	s32 nPassword1 = 0;
	size_t nPassword1Size = 0;

	if (SQLITE_OK != CUserRecord::AccessPassword(&inUser, CUserRecord::Meta::ColumnInfo::AccessMode::Get, &nPassword1, nPassword1Size))
	{
		return Error::IncorrectPassword;
	}
	if (IS_ZERO(nPassword1Size))
	{
		return Error::IncorrectPassword;
	}

	for (CSQLiteStatement<CUserRecord>::ExecResult::TRows::iterator it = pQResult->m_Results.begin(); it != pQResult->m_Results.end(); ++it)
	{
		CUserRecord& user = *it;
		s32 nPassword2 = 0;
		size_t nPassword2Size = 0;
		
		if (SQLITE_OK == CUserRecord::AccessPassword(&user, CUserRecord::Meta::ColumnInfo::AccessMode::Get, &nPassword2, nPassword2Size))
		{
			if ( IS_NOT_ZERO(nPassword2Size)
				&& (nPassword1Size == nPassword2Size)
				&& (nPassword1 == nPassword2) ) 
			{
				bFound = true;
				break;
			}
		}
	}
	if (IS_FALSE(bFound))
	{
		SysDebugPrintf("login incorrectpassword\n");
		return Error::IncorrectPassword;
	}

	SysDebugPrintf("login query success\n");

	//-- Success
	CSQLiteStatement<CUserRecord> ustmt(m_database);

	ustmt.Prepare("UPDATE Users SET lastLoginIP = @lastLoginIP, lastLoginTime = datetime('now'), bIsLoggedIn = 1 WHERE email = @email;");
	ustmt.Bind(&inUser);
		
	SysSmartPtr<CSQLiteStatement<CUserRecord>::ExecResult> pUResult = ustmt.Exec();
	if ( (SQLITE_OK != pQResult->m_nErrorCode) 
		&& (SQLITE_DONE != pQResult->m_nErrorCode) )
	{
		SysDebugPrintf("login update queryerror %d\n", pUResult->m_nErrorCode);
		return Error::QueryError;
	}

	SysDebugPrintf("login update success\n");

	return Error::Ok;
}


CUsersSQLTable::Error::Enum	CUsersSQLTable::Logout(CUserRecord& inUser)
{
	CSQLiteStatement<CUserRecord> ustmt(m_database);

	ustmt.Prepare("UPDATE Users SET lastLogoutTime = datetime('now'), bIsLoggedIn = 0 WHERE email = @email;");
	ustmt.Bind(&inUser);
		
	SysSmartPtr<CSQLiteStatement<CUserRecord>::ExecResult> pResult = ustmt.Exec();
	if ( (SQLITE_OK != pResult->m_nErrorCode) 
		&& (SQLITE_DONE != pResult->m_nErrorCode) )
	{
		return Error::QueryError;
	}

	return Error::Ok;
}


CUsersSQLTable::Error::Enum CUsersSQLTable::IsLoggedIn(CUserRecord& inUser) const
{
	CSQLiteStatement<CUserRecord> qstmt(m_database);

	qstmt.Prepare("SELECT bIsLoggedIn FROM Users WHERE email = @email;");
	qstmt.Bind(&inUser);

	SysSmartPtr<CSQLiteStatement<CUserRecord>::ExecResult> pResult = qstmt.Exec();
	if ( (SQLITE_OK != pResult->m_nErrorCode) 
		&& (SQLITE_DONE != pResult->m_nErrorCode) )
	{
		return Error::QueryError;
	}

	size_t nResults = pResult->m_Results.size();

	if (IS_ZERO(nResults))
	{
		return Error::NoSuchUser;
	}
	if (1 != nResults)
	{
		return Error::SanityFail;
	}

	for (CSQLiteStatement<CUserRecord>::ExecResult::TRows::iterator it = pResult->m_Results.begin(); it != pResult->m_Results.end(); ++it)
	{
		CUserRecord& user = *it;
		s32 nVal = 0;
		size_t nValSize = 0;
		
		if (SQLITE_OK == CUserRecord::AccessIsLoggedIn(&user, CUserRecord::Meta::ColumnInfo::AccessMode::Get, &nVal, nValSize))
		{
			if ( IS_NOT_ZERO(nValSize)
				&& (1 == nVal) )
			{
				return Error::LoggedIn;
			}
		}		
	}
	
	return Error::NotLoggedIn;
}



//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//
