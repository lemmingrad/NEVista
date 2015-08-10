#ifndef _SQLITE_RECORD_H_
#define _SQLITE_RECORD_H_
#pragma once


//----------------------------------------------------------//
// SQLITERECORD.H
//----------------------------------------------------------//
//-- Description			
// A base class used to derive a SQLite row object.
//----------------------------------------------------------//


#include "Types.h"
#include "FixedString.h"


//----------------------------------------------------------//
// DEFINES
//----------------------------------------------------------//


#define SQLITE_RECORD_COLUMN_NAME_SIZE			(32)


//----------------------------------------------------------//
// ENUMS
//----------------------------------------------------------//

//----------------------------------------------------------//
// FORWARD REFERENCES
//----------------------------------------------------------//

//----------------------------------------------------------//
// STRUCTS
//----------------------------------------------------------//

//----------------------------------------------------------//
// CLASSES
//----------------------------------------------------------//


class CSQLiteRecord
{
	public:

		struct Meta
		{
			struct ColumnInfo
			{
				struct Type
				{
					enum Enum
					{
						End = 0,
						Int,
						Float,
						Text, 
						Data
					};
				};

				struct AccessMode
				{
					enum Enum
					{
						Get = 0,
						Set
					};
				};

				typedef s32 (*ColumnAccessFunc)(void* pThis, AccessMode::Enum eMode, void* pData, size_t& nDataSize);

				FixedString<SQLITE_RECORD_COLUMN_NAME_SIZE>		m_strName;
				Type::Enum										m_eType;
				ColumnAccessFunc								m_pFunc;
			};

			const ColumnInfo* m_pColumns;
		};

		CSQLiteRecord() {}
		virtual ~CSQLiteRecord() {}
};


//----------------------------------------------------------//
// EXTERNALS
//----------------------------------------------------------//

//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//

#endif //_PACKET_H_