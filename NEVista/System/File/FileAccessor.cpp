//----------------------------------------------------------//
// FILEACCESSOR.CPP
//----------------------------------------------------------//
//-- Description
// CFileAccessor utility class.
//----------------------------------------------------------//


#include "FileAccessor.h"
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
// CFileAccessor::CFileAccessor
//----------------------------------------------------------//
CFileAccessor::CFileAccessor(CFileData* pData)
: m_pData(pData)
{
}


//----------------------------------------------------------//
// CFileAccessor::~CFileAccessor
//----------------------------------------------------------//
CFileAccessor::~CFileAccessor()
{
}


//----------------------------------------------------------//
// CFileAccessor::GetFileName
//----------------------------------------------------------//
const s8* CFileAccessor::GetFileName(void) const
{
	if (IS_PTR(m_pData))
	{
		return m_pData->GetFileName();
	}

	return "";
}


//----------------------------------------------------------//
// CFileAccessor::GetFileName
//----------------------------------------------------------//
SysString::Hash CFileAccessor::GetHash(void) const
{
	if (IS_PTR(m_pData))
	{
		return m_pData->GetHash();
	}

	return SysString::INVALID_HASH;
}


//----------------------------------------------------------//
// CFileAccessor::GetFileType
//----------------------------------------------------------//
CFileData::Type::Enum CFileAccessor::GetFileType(void) const
{
	if (IS_PTR(m_pData))
	{
		return m_pData->GetFileType();
	}

	return CFileData::Type::Unknown;
}


//----------------------------------------------------------//
// CFileAccessor::GetAccessMethod
//----------------------------------------------------------//
CFileData::AccessMethod::Enum CFileAccessor::GetAccessMethod(void) const
{
	if (IS_PTR(m_pData))
	{
		m_pData->GetAccessMethod();
	}

	return CFileData::AccessMethod::Unknown;
}


//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//

