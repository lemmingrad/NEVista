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
#include <sqlite3.h>
#include <list>


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
class CSQLiteRecord;


//----------------------------------------------------------//
// STRUCTS
//----------------------------------------------------------//

//----------------------------------------------------------//
// CLASSES
//----------------------------------------------------------//


template <class T>
class CSQLiteTable
{
	public:
	
		struct ExecResult
		{
			typedef std::list<T> TRows;

			ExecResult()
			: m_nErrorCode(SQLITE_OK)
			, m_strError(NULL)
			{
				m_Results.clear();
			}
	
			~ExecResult()
			{
				sqlite3_free(m_strError);
				printf("execresult dead\n");
				fflush(stdout);
			}
			
			TRows	m_Results;
			s32		m_nErrorCode;	
			s8*		m_strError;
		};

		class Statement
		{
			public:

				Statement(CSQLiteDatabase& database, CSQLiteTable<T>* pTable)
				: m_Database(database)
				, m_pTable(pTable)
				, m_pStatement(NULL)
				{
				}

				virtual ~Statement()
				{
					if (IS_PTR(m_pStatement))
					{
						sqlite3_finalize(m_pStatement);
						m_pStatement = NULL;
					}
					printf("statement dead\n");
					fflush(stdout);
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
					Reset();

					if (IS_PTR(m_pStatement))
					{
						if (IS_PTR(pRecord))
						{
							s32 nErrorCode = SQLITE_OK;
							
							for (u32 it = 0; it < T::sm_nInfoCount; ++it)
							{
								typename T::Info* pInfo = &T::sm_InfoList[it];

								//-- For each info, try and find a bind name for it in the command string.
								FixedString<33> strTemp("@");
								strTemp += pInfo->m_strName.ConstBuffer();
								s32 nIndex = sqlite3_bind_parameter_index(m_pStatement, strTemp.ConstBuffer());
								if (IS_NOT_ZERO(nIndex))
								{
									//-- Found a match.
									switch (pInfo->m_eType)
									{
										case T::Type::Int:
										{
											s32 nVal = 0;
											size_t nValSize = sizeof(nVal);
											if ( IS_PTR(pInfo->m_pGetFunc)
												&& (SQLITE_OK == pInfo->m_pGetFunc(pRecord, &nVal, nValSize)) )
											{
												nErrorCode = sqlite3_bind_int(m_pStatement, nIndex, nVal);
											}
											else
											{
												nErrorCode = sqlite3_bind_null(m_pStatement, nIndex);
											}
										}
										break;
										case T::Type::Float:
										{
											f32 fVal = 0;
											size_t nValSize = sizeof(fVal);
											if ( IS_PTR(pInfo->m_pGetFunc)
												&& (SQLITE_OK == pInfo->m_pGetFunc(pRecord, &fVal, nValSize)) )
											{
												nErrorCode = sqlite3_bind_double(m_pStatement, nIndex, fVal);
											}
											else
											{
												nErrorCode = sqlite3_bind_null(m_pStatement, nIndex);
											}
										}
										break;
										case T::Type::Text:
										{
											s8* pStr = NULL;
											size_t nValSize = sizeof(pStr);
											if ( IS_PTR(pInfo->m_pGetFunc)
												&& (SQLITE_OK == pInfo->m_pGetFunc(pRecord, &pStr, nValSize)) )
											{
												printf("text type: %s\n", pStr);
												fflush(stdout);
												
												nErrorCode = sqlite3_bind_text(m_pStatement, nIndex, pStr, -1, SQLITE_TRANSIENT);
											}
											else
											{
												printf("text type: NULL! %s\n", pStr);
												nErrorCode = sqlite3_bind_null(m_pStatement, nIndex);
											}
										}
										break;
										default:
										break;
									}
								}
								else
								{
									printf("no index!\n");
									fflush(stdout);
								}

								if (SQLITE_OK != nErrorCode)
								{
									//-- Error occured.
									break;
								}
							}
						}
					}

					return SQLITE_ERROR;
				}


				s32 Reset(void)
				{
					if (IS_PTR(m_pStatement))
					{
						return sqlite3_reset(m_pStatement);
					}

					return SQLITE_ERROR;
				}

				SysSmartPtr<ExecResult>	Exec(void)
				{
					ExecResult* pExecResult = new ExecResult();
					if (IS_PTR(pExecResult))
					{
						pExecResult->m_Results.clear();

						if (IS_PTR(m_pStatement))
						{
							s32 nErrorCode = sqlite3_step(m_pStatement);
							while (SQLITE_ROW == nErrorCode)
							{
								s32 nColumns = sqlite3_column_count(m_pStatement);

								printf("col count = %d\n", nColumns);
								fflush(stdout);

								s8** ppArgvs = new s8* [nColumns];
								s8** ppCols = new s8* [nColumns];
								if ( IS_PTR(ppArgvs) 
									&& IS_PTR(ppCols) )
								{
									for (s32 i = 0; i < nColumns; ++i)
									{
										ppArgvs[i] = (s8*)sqlite3_column_text(m_pStatement, i);
										printf("%s\n", ppArgvs[i]);
										fflush(stdout);

										ppCols[i] = (s8*)sqlite3_column_name(m_pStatement, i);
										printf("%s\n", ppCols[i]);
										fflush(stdout);
									}

									if (IS_NOT_ZERO(T::ExecCallback(pExecResult, nColumns, ppArgvs, ppCols)))
									{
										nErrorCode = SQLITE_ABORT;
									}
									else
									{
										nErrorCode = sqlite3_step(m_pStatement);
									}
								}

								SAFE_DELETE_ARRAY(ppArgvs);
								SAFE_DELETE_ARRAY(ppCols);
							}
						}
					}

					return SysSmartPtr<ExecResult>(pExecResult);
				}

			private:

				CSQLiteDatabase&	m_Database;
				CSQLiteTable<T>*	m_pTable;
				sqlite3_stmt*		m_pStatement;
		};
		
		CSQLiteTable(CSQLiteDatabase& database, const s8* strTableName)
		: m_Database(database)
		{
			m_strTableName.Set(strTableName);
		}

		virtual ~CSQLiteTable()
		{
		}

		SysSmartPtr<ExecResult>	Exec(const s8* strCommands)
		{
			ExecResult* pExecResult = new ExecResult();
			if (IS_PTR(pExecResult))
			{
				pExecResult->m_Results.clear();
				
				s32 nErrorCode = sqlite3_exec(m_Database.GetHandle(), strCommands, T::ExecCallback, pExecResult, &pExecResult->m_strError);
				if (SQLITE_OK == pExecResult->m_nErrorCode)
				{
					pExecResult->m_nErrorCode = nErrorCode;
				}
			}

			return SysSmartPtr<ExecResult>(pExecResult);
		}

		SysSmartPtr<Statement> NewStatement(void)
		{
			return SysSmartPtr<Statement>(new Statement(m_Database, this));
		}

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