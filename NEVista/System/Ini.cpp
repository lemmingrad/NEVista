//----------------------------------------------------------//
// INI.CPP
//----------------------------------------------------------//
//-- Description			
// Utility functions for reading and writing .ini files
// WARNING: Ini is going to be slow! It's not something you
// should use where speed is a factor.
//----------------------------------------------------------//


#include "Ini.h"
#include "Types.h"
#include "SysDebugLog.h"
#include "SysString.h"
#include "File/FileDirectTextReader.h"
#include "File/FileDirectTextWriter.h"


//----------------------------------------------------------//
// DEFINES
//----------------------------------------------------------//

//----------------------------------------------------------//
// GLOBALS
//----------------------------------------------------------//

//----------------------------------------------------------//
// CIni::Cini
//----------------------------------------------------------//
//-- Description			
// Wrapper for processing .ini files. Opens the
// file if it exists, reads file into ini data tree.
//----------------------------------------------------------//
CIni::CIni()
{
	m_IniGroupList.clear();
}


//----------------------------------------------------------//
// CIni::~CIni
//----------------------------------------------------------//
//-- Description			
// Ini file object destructor.
//----------------------------------------------------------//
CIni::~CIni()
{
	Shutdown();
}


//----------------------------------------------------------//
// CIni::Initialise
//----------------------------------------------------------//
//-- Description			
// Ini file initialiser. This must be called before anything
// else, to set up the data structure to hold Ini blocks.
//----------------------------------------------------------//
bool CIni::Initialise(void)
{
	m_IniGroupList.clear();
	
	return true;
}


//----------------------------------------------------------//
// CIni::Shutdown
//----------------------------------------------------------//
//-- Description			
// Ini file shutdown. Cleans up all used memory.
//----------------------------------------------------------//
bool CIni::Shutdown(void)
{
	m_IniGroupList.clear();

	return true;
}


//----------------------------------------------------------//
// CIni::Load
//----------------------------------------------------------//
//-- Description			
// Wrapper for processing .ini files. Opens the
// file if it exists, reads file into ini data tree.
//----------------------------------------------------------//
bool CIni::Load(const s8 *strFileName)
{
	gDebugLog.Printf("CIni::Load \"%s\"", strFileName);
	SCOPED_LOG_INDENT(gDebugLog);

	const SIniGroup* pCurrentGroup = NULL;

	FixedString<256> strLineBuffer;

	//-- open file for reading
	CFileDirectTextReader fileReader(strFileName);

	CFile::Error::Enum eResult = fileReader.Open();
	if (CFile::Error::Ok == eResult)
	{
		//-- File open
		while (fileReader.GetString(strLineBuffer.Buffer(), strLineBuffer.Size()))
		{
			s8* pLine = strLineBuffer.Buffer();
			size_t nLen = strLineBuffer.Length();

			if (IS_FALSE(SysString::IsEmpty(pLine)))
			{
				switch (pLine[0])
				{
					case '/':
					case ';':
					case '#':
					{
						//-- it's a comment line, ignore it.
						continue;
					}
					break;

					case '[':
					{
						if (nLen > 2)
						{
							s8* pTokeniserRecord;
							s8* pTitle = SysString::Strtok(pLine, "\r\n", pTokeniserRecord);
							if (IS_FALSE(SysString::IsEmpty(pTitle)))
							{
								size_t nTitleLen = SysString::Strlen(pTitle);
								if (pTitle[nTitleLen-1] == ']')
								{
									//-- Line starts with [ and ends with ]. This is a group header.
									pTitle[nTitleLen-1] = '\0';
									pTitle++;

									pCurrentGroup = FindGroup(pTitle);
									if (IS_NULL_PTR(pCurrentGroup))
									{
										pCurrentGroup = CreateGroup(pTitle);
									}
								}
							}
						}
					}
					break;

					default:
					{	
						//-- it could be a tuple line...	
						if (IS_PTR(pCurrentGroup))
						{
							s8* pTokeniserRecord;
							s8* pName = SysString::Strtok(pLine, "=\r\n", pTokeniserRecord);
							if (IS_FALSE(SysString::IsEmpty(pName)))
							{
								s8* pValue = SysString::Strtok(NULL, "\r\n", pTokeniserRecord);
								if (IS_FALSE(SysString::IsEmpty(pValue)))
								{
									WriteString(pCurrentGroup->m_strName.ConstBuffer(), pName, pValue);
								}
							}
						}
					}
					break;
				}
			}
		}

		fileReader.Close();

		gDebugLog.Printf("CIni load complete. (OK)");
		return true;
	}

	gDebugLog.Printf("Could not open file.");
	return false;
}


//----------------------------------------------------------//
// CIni::Save (Public)
//----------------------------------------------------------//
//-- Description			
// Writes ini data tree into a .ini file
//----------------------------------------------------------//
bool CIni::Save(const s8* strFileName)
{
	gDebugLog.Printf("CIni::Save \"%s\"", strFileName);
	SCOPED_LOG_INDENT(gDebugLog);

	FixedString<256> strLineBuffer;

	//-- open file for writing
	CFileDirectTextWriter fireWriter(strFileName);

	CFile::Error::Enum eResult = fireWriter.Open();
	if (CFile::Error::Ok == eResult)
	{
		fireWriter.Printf("## %s\n", strFileName);
		fireWriter.Printf("## Generated by in-game Ini library.\n");
		fireWriter.Printf("## File parser is extreme ropey and will break if file is not formatted correctly.\n");
		fireWriter.Printf("## Do not attempt to modify unless you know what you are doing.\n");

		for (TIniGroupList::const_iterator cit = m_IniGroupList.begin(); cit != m_IniGroupList.end(); ++cit)
		{
			const SIniGroup* pGroup = &(*cit);
			if (IS_PTR(pGroup))
			{
				fireWriter.Printf("\n[%s]\n", pGroup->m_strName.ConstBuffer());

				for (TIniTupleList::const_iterator cittu = pGroup->m_IniTupleList.begin(); cittu != pGroup->m_IniTupleList.end(); ++cittu)
				{
					const SIniTuple* pTuple = &(*cittu);
					if (IS_PTR(pTuple))
					{
						fireWriter.Printf("%s=%s\n", pTuple->m_strVariableName.ConstBuffer(), pTuple->m_strValue.ConstBuffer());
					}
				}
			}
		}

		fireWriter.Close();
	}
	else
	{
		gDebugLog.Printf("Could not save file.");
	}

	gDebugLog.Printf("CIni save complete. (OK)");

	return true;
}


//----------------------------------------------------------//
// CIni::FindGroup (Private)
//----------------------------------------------------------//
//-- Description			
// Finds an existing group in the ini data tree, or returns 
// NULL if it does not exist.
//----------------------------------------------------------//
CIni::SIniGroup* CIni::FindGroup(const s8* strGroupName)
{
	gDebugLog.Printf("CIni::FindGroup");
	SCOPED_LOG_INDENT(gDebugLog);
	
	//-- run through the list to see if group exists.
	for (TIniGroupList::iterator it = m_IniGroupList.begin(); it != m_IniGroupList.end(); ++it)
	{
		SIniGroup* pGroup = &(*it);
		if (IS_PTR(pGroup))
		{
			if (!SysString::Strncmp(pGroup->m_strName.ConstBuffer(), strGroupName, pGroup->m_strName.Size()))
			{
				//-- Found
				gDebugLog.Printf("Found existing group: \"%s\"", pGroup->m_strName.ConstBuffer());
	
				return pGroup;
			}
		}
	}

	gDebugLog.Printf("Could not find group \"%s\"", strGroupName);
	
	return NULL;
}


//----------------------------------------------------------//
// CIni::CreateGroup (Private)
//----------------------------------------------------------//
//-- Description			
// Adds a new group in the ini data tree
//----------------------------------------------------------//
CIni::SIniGroup* CIni::CreateGroup(const s8* strGroupName)
{
	gDebugLog.Printf("CIni::CreateGroup");
	SCOPED_LOG_INDENT(gDebugLog);

	m_IniGroupList.push_back(SIniGroup());

	SIniGroup* pGroup = &(m_IniGroupList.back());
	if (IS_PTR(pGroup))
	{
		pGroup->m_IniTupleList.clear();

		pGroup->m_strName.Set(strGroupName);
		if (pGroup->m_strName.Length() != SysString::Strlen(strGroupName))
		{
			gDebugLog.Printf("Group name \"%s\" truncated to \"%s\".", strGroupName, pGroup->m_strName.ConstBuffer());
		}

		gDebugLog.Printf("Created new group: \"%s\"", pGroup->m_strName.ConstBuffer());
	}

	return pGroup;
}


//----------------------------------------------------------//
// CIni::RemoveGroup (Public)
//----------------------------------------------------------//
//-- Description			
// Deletes a group from the ini data tree, and all tuples
// belonging to that group.
//----------------------------------------------------------//
void CIni::RemoveGroup(const s8* strGroupName)
{
	gDebugLog.Printf("CIni::RemoveGroup");
	SCOPED_LOG_INDENT(gDebugLog);

	//-- find group
	for (TIniGroupList::iterator it = m_IniGroupList.begin(); it != m_IniGroupList.end(); ++it)
	{
		SIniGroup* pGroup = &(*it);
		if (IS_PTR(pGroup))
		{
			if (!SysString::Strncmp(pGroup->m_strName.ConstBuffer(), strGroupName, pGroup->m_strName.Size()))
			{
				//-- Found a matching group, destroy the tuple list
				pGroup->m_IniTupleList.clear();

				m_IniGroupList.erase(it);

				gDebugLog.Printf("Removed group \"%s\".", strGroupName);
			}
		}
	}

	gDebugLog.Printf("Finished. (OK)");
}


//----------------------------------------------------------//
// CIni::FindTuple (Private)
//----------------------------------------------------------//
//-- Description			
// Finds an existing tuple in the specified group, or returns 
// NULL if it does not exist.
//----------------------------------------------------------//
CIni::SIniTuple* CIni::FindTuple(CIni::SIniGroup* pGroup, const s8* strVariableName)
{
	gDebugLog.Printf("CIni::FindTuple");
	SCOPED_LOG_INDENT(gDebugLog);

	if (IS_PTR(pGroup))
	{
		//-- Run through the list to see if group exists.
		for (TIniTupleList::iterator it = pGroup->m_IniTupleList.begin(); it != pGroup->m_IniTupleList.end(); ++it)
		{
			SIniTuple* pTuple= &(*it);
			if (IS_PTR(pTuple))
			{
				if (!SysString::Strncmp(pTuple->m_strVariableName.ConstBuffer(), strVariableName, pTuple->m_strVariableName.Size()))
				{
					//-- Found
					gDebugLog.Printf("Found existing tuple: \"%s\"", pTuple->m_strVariableName.ConstBuffer());
		
					return pTuple;
				}
			}
		}
	}

	gDebugLog.Printf("Could not find tuple \"%s\"", strVariableName);

	return NULL;
}


//----------------------------------------------------------//
// CIni::CreateTuple (Private)
//----------------------------------------------------------//
//-- Description			
// Adds a new tuple to the specified group
//----------------------------------------------------------//
CIni::SIniTuple* CIni::CreateTuple(CIni::SIniGroup* pGroup, const s8* strVariableName, const s8* strValue)
{
	gDebugLog.Printf("CIni::CreateTuple");
	SCOPED_LOG_INDENT(gDebugLog);

	if (IS_PTR(pGroup))
	{
		pGroup->m_IniTupleList.push_back(SIniTuple());

		SIniTuple* pTuple = &(pGroup->m_IniTupleList.back());
		if (IS_PTR(pTuple))
		{
			pTuple->m_strVariableName.Set(strVariableName);
			if (pTuple->m_strVariableName.Length() != SysString::Strlen(strVariableName))
			{
				gDebugLog.Printf("Tuple name \"%s\" truncated to \"%s\".", strVariableName, pTuple->m_strVariableName.ConstBuffer());
			}

			pTuple->m_strValue.Set(strValue);
			if (pTuple->m_strValue.Length() != SysString::Strlen(strValue))
			{
				gDebugLog.Printf("Tuple value \"%s\" truncated to \"%s\".", strValue, pTuple->m_strValue.ConstBuffer());
			}

			gDebugLog.Printf("Created new tuple: \"%s=%s\"", pTuple->m_strVariableName.ConstBuffer(), pTuple->m_strValue.ConstBuffer());
		}

		return pTuple;
	}

	return NULL;
}


//----------------------------------------------------------//
// CIni::RemoveTuple (Public)
//----------------------------------------------------------//
//-- Description			
// Deletes a tuple from a specified group
//----------------------------------------------------------//
void CIni::RemoveTuple(const s8* strGroupName, const s8* strVariableName)
{
	gDebugLog.Printf("CIni::RemoveTuple");
	SCOPED_LOG_INDENT(gDebugLog);

	//-- find group
	SIniGroup* pGroup = FindGroup(strGroupName);
	if (IS_PTR(pGroup))
	{
		//-- find tuple
		RemoveTuple(pGroup, strVariableName);
	}

	gDebugLog.Printf("Finished. (OK)");
}

void CIni::RemoveTuple(CIni::SIniGroup* pGroup, const s8* strVariableName)
{
	gDebugLog.Printf("CIni::RemoveTuple");
	SCOPED_LOG_INDENT(gDebugLog);

	//-- find group
	if (IS_PTR(pGroup))
	{
		for (TIniTupleList::iterator it = pGroup->m_IniTupleList.begin(); it != pGroup->m_IniTupleList.end(); ++it)
		{
			SIniTuple* pTuple= &(*it);
			if (IS_PTR(pTuple))
			{
				if (!SysString::Strncmp(pTuple->m_strVariableName.ConstBuffer(), strVariableName, pTuple->m_strVariableName.Size()))
				{
					//-- Found a matching group, destroy the tuple list
					pGroup->m_IniTupleList.erase(it);

					gDebugLog.Printf("Removed tuple \"%s\" from group \"%s\".", strVariableName, pGroup->m_strName.ConstBuffer());
				}
			}
		}
	}

	gDebugLog.Printf("Finished. (OK)");
}


//----------------------------------------------------------//
// CIni::ReadInt (integers)
//----------------------------------------------------------//
//-- Description			
// Returns the value of a specified variable in a specified
// group.
//----------------------------------------------------------//
s32 CIni::ReadInt(const s8* strGroupName, const s8* strVariableName)
{
	gDebugLog.Printf("CIni::ReadInt");
	SCOPED_LOG_INDENT(gDebugLog);

	f32 fValue = ReadFloat(strGroupName, strVariableName);

	gDebugLog.Printf("Completed.");

	return (s32)fValue;
}


//----------------------------------------------------------//
// CIni::ReadFloat (floats)
//----------------------------------------------------------//
//-- Description			
// Returns the value of a specified variable in a specified
// group.
//----------------------------------------------------------//
f32 CIni::ReadFloat(const s8* strGroupName, const s8* strVariableName)
{
	gDebugLog.Printf("CIni::ReadFloat");
	SCOPED_LOG_INDENT(gDebugLog);

	const s8* strValue = ReadString(strGroupName, strVariableName);

	gDebugLog.Printf("Completed.");

	if (IS_FALSE(SysString::IsEmpty(strValue)))
	{
		const u32 nTrueWords = 6;
		const s8* strTrueWordList[] = {"TRUE", "True", "true", "YES", "Yes", "yes"};
		const u32 nFalseWords = 6;
		const s8* strFalseWordList[] = {"FALSE", "False", "false", "NO", "No", "no"};
		for (u32 i = 0; i < nTrueWords; ++i)
		{
			if (!SysString::Strncmp(strValue, strTrueWordList[i], INI_MAX_TUPLE_VALUE_LENGTH))
			{
				return 1.0f;
			}
		}
		for (u32 j = 0; j < nFalseWords; ++j)
		{
			if (!SysString::Strncmp(strValue, strFalseWordList[j], INI_MAX_TUPLE_VALUE_LENGTH))
			{
				return 0.0f;
			}
		}

		f32 fValue = SysString::Atof(strValue);
		return fValue;
	}

	return 0.0f;
}


//----------------------------------------------------------//
// CIni::ReadString (strings)
//----------------------------------------------------------//
//-- Description			
// Returns the value of a specified variable in a specified
// group.
//----------------------------------------------------------//
const s8* CIni::ReadString(const s8* strGroupName, const s8* strVariableName)
{
	gDebugLog.Printf("CIni::ReadString");
	SCOPED_LOG_INDENT(gDebugLog);

	SIniGroup* pGroup = FindGroup(strGroupName);
	if (IS_PTR(pGroup))
	{
		SIniTuple* pTuple = FindTuple(pGroup, strVariableName);
		if (IS_PTR(pTuple))
		{
			//-- found a match
			gDebugLog.Printf("Completed. (OK)");

			return pTuple->m_strValue.ConstBuffer();
		}
	}

	gDebugLog.Printf("No result. (OK)");

	return NULL;
}


//----------------------------------------------------------//
// CIni::WriteInt (integers)
//----------------------------------------------------------//
//-- Description			
// Writes a new value for the ini data
// Automatically creates new groups if necessary, or 
// overwrites existing tuples with the new value.
//----------------------------------------------------------//
void CIni::WriteInt(const s8* strGroupName, const s8* strVariableName, s32 nValue)
{
	gDebugLog.Printf("CIni::WriteInt");
	SCOPED_LOG_INDENT(gDebugLog);

	FixedString<INI_MAX_TUPLE_VALUE_LENGTH> strBuffer;

	SysString::Sprintf(strBuffer.Buffer(), strBuffer.Size(), "%d", nValue);

	WriteString(strGroupName, strVariableName, strBuffer.ConstBuffer());

	gDebugLog.Printf("Completed.");
}


//----------------------------------------------------------//
// CIni::WriteFloat (floats)
//----------------------------------------------------------//
//-- Description			
// Writes a new value for the ini data
// Automatically creates new groups if necessary, or 
// overwrites existing tuples with the new value.
//----------------------------------------------------------//
void CIni::WriteFloat(const s8* strGroupName, const s8* strVariableName, f32 fValue)
{
	gDebugLog.Printf("CIni::WriteFloat");
	SCOPED_LOG_INDENT(gDebugLog);

	FixedString<INI_MAX_TUPLE_VALUE_LENGTH> strBuffer;

	SysString::Sprintf(strBuffer.Buffer(), strBuffer.Size(), "%f", fValue);

	WriteString(strGroupName, strVariableName, strBuffer.ConstBuffer());

	gDebugLog.Printf("Completed.");
}


//----------------------------------------------------------//
// CIni::WriteString (strings)
//----------------------------------------------------------//
//-- Description			
// Writes a new value for the ini data
// Automatically creates new groups if necessary, or 
// overwrites existing tuples with the new value.
//----------------------------------------------------------//
void CIni::WriteString(const s8* strGroupName, const s8* strVariableName, const s8* strValue)
{
	gDebugLog.Printf("CIni::WriteString");
	SCOPED_LOG_INDENT(gDebugLog);

	SIniGroup* pGroup = FindGroup(strGroupName);
	if (IS_PTR(pGroup))
	{
		//-- Group already exists.
		gDebugLog.Printf("Group \"%s\" found.", strGroupName);
	}
	else
	{
		pGroup = CreateGroup(strGroupName);

		gDebugLog.Printf("Created new group \"%s\".", strGroupName);
	}

	SIniTuple* pTuple = FindTuple(pGroup, strVariableName);
	if (IS_PTR(pTuple))
	{
		//-- Tuple already exists in this group.
		pTuple->m_strValue.Set(strValue);

		gDebugLog.Printf("Modifying existing tuple \"%s=%s\".", pTuple->m_strVariableName.ConstBuffer(), pTuple->m_strValue.ConstBuffer());
	}
	else
	{
		//-- tuple does not exist in this group, so make one.
		pTuple = CreateTuple(pGroup, strVariableName, strValue);

		gDebugLog.Printf("Created new tuple \"%s=%s\".", strVariableName, strValue);
	}

	gDebugLog.Printf("Completed. (OK)");
}


//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//
