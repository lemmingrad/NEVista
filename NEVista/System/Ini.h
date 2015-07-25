#ifndef _INI_H_
#define _INI_H_
#pragma once

//----------------------------------------------------------//
// INI.H
//----------------------------------------------------------//
//-- Description			
// Utility functions for reading and writing .ini files
// WARNING: Ini is going to be slow! It's not something you
// should use where speed is a factor.
//----------------------------------------------------------//


#include "Types.h"
#include "FixedString.h"
#include <list>


//----------------------------------------------------------//
// DEFINES
//----------------------------------------------------------//


#define INI_MAX_GROUP_NAME_LENGTH		(32)
#define INI_MAX_TUPLE_NAME_LENGTH		(32)
#define INI_MAX_TUPLE_VALUE_LENGTH		(32)


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


class CIni
{
	public:

		struct SIniTuple
		{
			FixedString<INI_MAX_TUPLE_NAME_LENGTH> 		m_strVariableName;
			FixedString<INI_MAX_TUPLE_VALUE_LENGTH>		m_strValue;
		};

		struct SIniGroup
		{
			FixedString<INI_MAX_GROUP_NAME_LENGTH>		m_strName;
			std::list<SIniTuple>						m_IniTupleList;
		};

		typedef std::list<SIniTuple> TIniTupleList;
		typedef std::list<SIniGroup> TIniGroupList;

		CIni();
		~CIni();

		bool						Initialise(void);
		bool						Shutdown(void);

		const s8*					ReadString(const s8* strGroupName, const s8* strVariableName);
		s32							ReadInt(const s8* strGroupName, const s8* strVariableName);
		f32							ReadFloat(const s8* strGroupName, const s8* strVariableName);

		void						WriteString(const s8* strGroupName, const s8* strVariableName, const s8* strValue);
		void						WriteInt(const s8* strGroupName, const s8* strVariableName, s32 nValue);
		void						WriteFloat(const s8* strGroupName, const s8* strVariableName, f32 fValue);

		bool						Load(const s8* strFileName);
		bool						Save(const s8* strFileName);

		void						RemoveGroup(const s8* strGroupName);
		void						RemoveTuple(SIniGroup* pGroup, const s8* strVariableName);
		void						RemoveTuple(const s8* strGroupName, const s8* strVariableName);

	private:

		std::list<SIniGroup>		m_IniGroupList;

		SIniGroup*					FindGroup(const s8* strGroupName);
		SIniGroup*					CreateGroup(const s8* strGroupName);

		SIniTuple*					FindTuple(SIniGroup* pGroup, const s8* strVariableName);
		SIniTuple*					CreateTuple(SIniGroup* pGroup, const s8* strVariableName, const s8* strValue);
};


//----------------------------------------------------------//
// EXTERNALS
//----------------------------------------------------------//

//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//

#endif