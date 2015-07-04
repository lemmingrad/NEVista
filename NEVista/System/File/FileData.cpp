//----------------------------------------------------------//
// FILEDATA.CPP
//----------------------------------------------------------//
//-- Description
// CFileData is a structure to be held in lists within the 
// FileManager containing the actual file status. 
// This data should not be accessible directly:
// Use a FileAccessor to read/write to the file, and let the 
// FileManager do all the updating via a FileProcessor.
//----------------------------------------------------------//


#include "FileData.h"
#include "Types.h"
#include "SysString.h"


//----------------------------------------------------------//
// DEFINES
//----------------------------------------------------------//

//----------------------------------------------------------//
// GLOBALS
//----------------------------------------------------------//


//----------------------------------------------------------//
// CFileData::CFileData
//----------------------------------------------------------//
CFileData::CFileData(const s8* strFileName, CFileData::Type::Enum eType, CFileData::AccessMethod::Enum eAccess)
: m_strFileName()
, m_nHash(SysString::INVALID_HASH)
, m_eFileType(eType)
, m_eAccessMethod(eAccess)
{
	m_strFileName.Set(strFileName);
	m_nHash = SysString::GenerateHash(strFileName);
}


//----------------------------------------------------------//
// CFileData::~CFileData
//----------------------------------------------------------//
CFileData::~CFileData()
{
}


//----------------------------------------------------------//
// CFileData::Validate
//----------------------------------------------------------//
bool CFileData::Validate(CFileData::Type::Enum eType, CFileData::AccessMethod::Enum eAccess) const
{
	if (SysString::INVALID_HASH != GetHash())
	{
		switch (eType)
		{
			case Type::Any:
			break;
			default:
			{
				if (eType != GetFileType())
				{
					return false;
				}
			}
			break;
		}

		switch (eAccess)
		{
			case AccessMethod::Any:
			break;
			default:
			{
				if (eAccess != GetAccessMethod())
				{
					return false;
				}
			}
			break;
		}

		return true;
	}

	return false;
}


//----------------------------------------------------------//
// CFileData::GetFileType
//----------------------------------------------------------//
CFileData::Type::Enum CFileData::GetFileType(void) const
{
	return m_eFileType;
}


//----------------------------------------------------------//
// CFileData::GetAccessMethod
//----------------------------------------------------------//
CFileData::AccessMethod::Enum CFileData::GetAccessMethod(void) const
{
	return m_eAccessMethod;
}


//----------------------------------------------------------//
// CFileData::GetFileName
//----------------------------------------------------------//
const s8* CFileData::GetFileName(void) const
{
	return m_strFileName.ConstBuffer();
}


//----------------------------------------------------------//
// CFileData::GetHash
//----------------------------------------------------------//
SysString::Hash CFileData::GetHash(void) const
{
	return m_nHash;
}


//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//

