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
#include <vector>


//----------------------------------------------------------//
// DEFINES
//----------------------------------------------------------//

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


class SQLiteRecord
{
	public:

		struct Type
		{
			enum Enum
			{
				Int = 0,
				Float,
				Text, 
				Data
			};
		};

		typedef s32 (*GetFunc)(void* pThis, void* pData, size_t& nDataSize);
		typedef s32 (*SetFunc)(void* pThis, void* pData, size_t nDataSize);

		struct Info
		{
			FixedString<32>		m_strName;
			Type::Enum			m_eType;
			GetFunc				m_pGetFunc;
			SetFunc				m_pSetFunc;
		};

//		typedef std::vector<Info> TColumnInfoList;

		SQLiteRecord()
		{
		}

		virtual ~SQLiteRecord()
		{
		}

	protected:
};


//----------------------------------------------------------//
// EXTERNALS
//----------------------------------------------------------//

//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//

#endif //_PACKET_H_