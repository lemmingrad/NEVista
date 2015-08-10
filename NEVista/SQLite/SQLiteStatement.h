#ifndef _SQLITE_STATEMENT_H_
#define _SQLITE_STATEMENT_H_
#pragma once


//----------------------------------------------------------//
// SQLITESTATEMENT.H
//----------------------------------------------------------//
//-- Description			
// An SQLite prepared statement wrapper
//----------------------------------------------------------//


#include "Types.h"
#include "SysString.h"
#include "FixedString.h"
#include "SysSmartPtr.h"
#include <list>
#include <sqlite3.h>


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


template <class T>
class CSQLiteStatement
{
	public:

		struct ExecResult
		{
			typedef std::list<T> TRows;

			ExecResult()
			: m_nErrorCode(SQLITE_OK)
			, m_pStrError(NULL)
			{
				m_Results.clear();
			}
	
			~ExecResult()
			{
			}
			
			TRows	m_Results;
			s32		m_nErrorCode;	
			s8*		m_pStrError;
		};

		CSQLiteStatement(CSQLiteDatabase& database)
		: m_Database(database)
		, m_pStatement(NULL)
		{
		}

		~CSQLiteStatement()
		{
			if (IS_PTR(m_pStatement))
			{
				sqlite3_finalize(m_pStatement);
				m_pStatement = NULL;
			}
		}

		s32 Prepare(const s8* strCommand)
		{
			if (IS_PTR(m_pStatement))
			{
				sqlite3_finalize(m_pStatement);
				m_pStatement = NULL;
			}
			
			return sqlite3_prepare_v2(m_Database.GetHandle(), strCommand, -1, &m_pStatement, NULL);
		}

		s32 Bind(T* pRecord)
		{
			if ( IS_NULL_PTR(m_pStatement) 
				|| IS_NULL_PTR(pRecord) )
			{
				return SQLITE_ERROR;
			}

			Reset();

			s32 nErrorCode = SQLITE_OK;
			
			const typename T::Meta& meta = T::GetMetaData();

			const typename T::Meta::ColumnInfo* pColumnInfo = meta.m_pColumns;
			while ( IS_PTR(pColumnInfo) 
				&& (T::Meta::ColumnInfo::Type::End != pColumnInfo->m_eType) )
			{
				//-- Does meta column info have an access function?
				if (IS_NULL_PTR(pColumnInfo->m_pFunc))
				{
					//-- Nope. Continue to next meta column info.
					++pColumnInfo;
					continue;
				}

				//-- Try to find a bind (either @name or :name) in the command string.
				FixedString<33> strTemp("@");
				strTemp += pColumnInfo->m_strName.ConstBuffer();

				s32 nIndex = sqlite3_bind_parameter_index(m_pStatement, strTemp.ConstBuffer());
				if (IS_ZERO(nIndex))
				{
					//-- No match for @name. Try :name.
					strTemp.Buffer()[0] = ':';
					nIndex = sqlite3_bind_parameter_index(m_pStatement, strTemp.ConstBuffer());
					if (IS_ZERO(nIndex))
					{
						//-- No match for :name either. Try $name.
						strTemp.Buffer()[0] = '$';
						nIndex = sqlite3_bind_parameter_index(m_pStatement, strTemp.ConstBuffer());
						if (IS_ZERO(nIndex))
						{
							//-- No match for $name either. Continue to next meta column info.
							++pColumnInfo;
							continue;
						}
					}
				}

				//-- Found a match.
				switch (pColumnInfo->m_eType)
				{
					case T::Meta::ColumnInfo::Type::Int:
					{
						s32 nVal = 0;
						size_t nValSize = sizeof(nVal);

						nErrorCode = pColumnInfo->m_pFunc(pRecord, T::Meta::ColumnInfo::AccessMode::Get, &nVal, nValSize);
						if (SQLITE_OK == nErrorCode)
						{
							nErrorCode = sqlite3_bind_int(m_pStatement, nIndex, nVal);
						}
					}
					break;

					case T::Meta::ColumnInfo::Type::Float:
					{
						f32 fVal = 0;
						size_t nValSize = sizeof(fVal);
					
						nErrorCode = pColumnInfo->m_pFunc(pRecord, T::Meta::ColumnInfo::AccessMode::Get, &fVal, nValSize);
						if (SQLITE_OK == nErrorCode)
						{
							nErrorCode = sqlite3_bind_double(m_pStatement, nIndex, fVal);
						}
					}
					break;

					case T::Meta::ColumnInfo::Type::Text:
					{
						s8* pText = NULL;
						size_t nTextBufferSize = 0;

						nErrorCode = pColumnInfo->m_pFunc(pRecord, T::Meta::ColumnInfo::AccessMode::Get, &pText, nTextBufferSize);
						if (SQLITE_OK == nErrorCode)
						{
							nErrorCode = sqlite3_bind_text(m_pStatement, nIndex, pText, SysString::Strlen(pText), SQLITE_TRANSIENT);
						}
					}
					break;

					case T::Meta::ColumnInfo::Type::Data:
					{
						u8* pData = NULL;
						size_t nDataSize = 0;

						nErrorCode = pColumnInfo->m_pFunc(pRecord, T::Meta::ColumnInfo::AccessMode::Get, &pData, nDataSize);
						if (SQLITE_OK == nErrorCode)
						{
							nErrorCode = sqlite3_bind_blob(m_pStatement, nIndex, pData, nDataSize, SQLITE_TRANSIENT);
						}
					}
					break;

					default:
					break;
				}

				if (SQLITE_OK != nErrorCode)
				{
					//-- something went wrong somewhere.
					break;
				}

				++pColumnInfo;
			}

			return nErrorCode;
		}
		
		//-- Exec:
		//-- Process a single prepared, parameterized command.
		//-- Pros: 
		//--   Can use a bound row class for parameter input.
		//--   Results include a list of new row class objects.
		//-- Cons: 
		//--   Only supports a single command. Subsequent commands in command string are ignored.
		//--   No error string in result.
		SysSmartPtr<ExecResult> Exec(void)
		{
			ExecResult* pExecResult = new ExecResult();

			if (IS_NULL_PTR(m_pStatement)) 
			{
				//-- Error. No active statement ptr
				pExecResult->m_nErrorCode = SQLITE_ERROR;
				return SysSmartPtr<ExecResult>(pExecResult);
			}

			const typename T::Meta& meta = T::GetMetaData();

			s32 nErrorCode = sqlite3_step(m_pStatement);
			while (SQLITE_ROW == nErrorCode)
			{
				s32 nRowColumns = sqlite3_column_count(m_pStatement);
				if (IS_ZERO(nRowColumns))
				{
					//-- No columns, try next row.
					nErrorCode = sqlite3_step(m_pStatement);
					continue;
				}

				pExecResult->m_Results.push_back(T());
				T& row = pExecResult->m_Results.back();

				for (s32 i = 0; i < nRowColumns; ++i)
				{
					s8* pName = (s8*)sqlite3_column_name(m_pStatement, i);

					const typename T::Meta::ColumnInfo* pColumnInfo = meta.m_pColumns;
					while ( IS_PTR(pColumnInfo)
						&& (T::Meta::ColumnInfo::Type::End != pColumnInfo->m_eType) )
					{
						if ( IS_NULL_PTR(pColumnInfo->m_pFunc)
							|| IS_NOT_ZERO(SysString::Strcmp(pName, pColumnInfo->m_strName.ConstBuffer())) )
						{
							//-- Not match: continue next meta column data
							++pColumnInfo;
							continue;
						}

						//-- Match.
						//-- Process data type
						switch (pColumnInfo->m_eType)
						{
							case T::Meta::ColumnInfo::Type::Int:
							{
								s32 nVal = sqlite3_column_int(m_pStatement, i);
								size_t nValSize = sizeof(nVal);

								nErrorCode = pColumnInfo->m_pFunc(&row, T::Meta::ColumnInfo::AccessMode::Set, &nVal, nValSize);
							}
							break;

							case T::Meta::ColumnInfo::Type::Float:
							{
								f32 fVal = (f32)sqlite3_column_double(m_pStatement, i);
								size_t nValSize = sizeof(fVal);
								
								nErrorCode = pColumnInfo->m_pFunc(&row, T::Meta::ColumnInfo::AccessMode::Set, &fVal, nValSize);
							}
							break;

							case T::Meta::ColumnInfo::Type::Text:
							{
								s8* pText = (s8*)sqlite3_column_text(m_pStatement, i);
								//-- nTextBufferSize is the size of the string, or length+1.
								size_t nTextBufferSize = SysString::Strlen(pText)+1;

								nErrorCode = pColumnInfo->m_pFunc(&row, T::Meta::ColumnInfo::AccessMode::Set, &pText, nTextBufferSize);
							}
							break;

							case T::Meta::ColumnInfo::Type::Data:
							{
								u8* pData = (u8*)sqlite3_column_blob(m_pStatement, i);
								size_t nDataSize = sqlite3_column_bytes(m_pStatement, i);

								nErrorCode = pColumnInfo->m_pFunc(&row, T::Meta::ColumnInfo::AccessMode::Set, &pData, nDataSize);
							}
							break;

							default:
							break;
						}

						++pColumnInfo;
					}
				}

				nErrorCode = sqlite3_step(m_pStatement);
			}

			if (SQLITE_OK == pExecResult->m_nErrorCode)
			{
				pExecResult->m_nErrorCode = nErrorCode;
			}
			
			return SysSmartPtr<ExecResult>(pExecResult);
		}

		s32 Reset(void)
		{
			if (IS_PTR(m_pStatement))
			{
				return sqlite3_reset(m_pStatement);
			}

			return SQLITE_OK;
		}

	private:

		CSQLiteDatabase&	m_Database;
		sqlite3_stmt*		m_pStatement;
};


//----------------------------------------------------------//
// EXTERNALS
//----------------------------------------------------------//

//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//

#endif //_SQLITE_STATEMENT_H_