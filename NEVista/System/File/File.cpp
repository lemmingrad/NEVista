//----------------------------------------------------------//
// FILE.CPP
//----------------------------------------------------------//
//-- Description
// CFile base class.
//----------------------------------------------------------//


#include "File.h"
#include "Types.h"
#include "SysString.h"
#include "FixedString.h"


//----------------------------------------------------------//
// DEFINES
//----------------------------------------------------------//

//----------------------------------------------------------//
// GLOBALS
//----------------------------------------------------------//

//----------------------------------------------------------//
// CFile::CFile
//----------------------------------------------------------//
CFile::CFile(const s8* strFileName, IFile::Type::Enum eType, IFile::AccessMethod::Enum eAccess)
: m_strFileName()
, m_nHash(SysString::INVALID_HASH)
, m_eFileType(eType)
, m_eAccessMethod(eAccess)
{
	m_strFileName.Set(strFileName);
	m_nHash = SysString::GenerateHash(strFileName);
}


//----------------------------------------------------------//
// CFile::CFile
//----------------------------------------------------------//
CFile::CFile(const IFixedString& strFileName, IFile::Type::Enum eType, IFile::AccessMethod::Enum eAccess)
	: m_strFileName()
	, m_nHash(SysString::INVALID_HASH)
	, m_eFileType(eType)
	, m_eAccessMethod(eAccess)
{
	m_strFileName.Set(strFileName);
	m_nHash = SysString::GenerateHash(strFileName.ConstBuffer());
}


//----------------------------------------------------------//
// CFile::~CFile
//----------------------------------------------------------//
CFile::~CFile()
{
}


//----------------------------------------------------------//
// CFile::GetFileType
//----------------------------------------------------------//
IFile::Type::Enum CFile::GetFileType(void) const
{
	return m_eFileType;
}


//----------------------------------------------------------//
// CFile::GetAccessMethod
//----------------------------------------------------------//
IFile::AccessMethod::Enum CFile::GetAccessMethod(void) const
{
	return m_eAccessMethod;
}


//----------------------------------------------------------//
// CFile::GetFileName
//----------------------------------------------------------//
const IFixedString& CFile::GetFileName(void) const
{
	return m_strFileName;
}


//----------------------------------------------------------//
// CFile::GetHash
//----------------------------------------------------------//
SysString::Hash CFile::GetHash(void) const
{
	return m_nHash;
}


//----------------------------------------------------------//
// CFile::IsTypeAccess
//----------------------------------------------------------//
bool CFile::IsTypeAccess(IFile::Type::Enum eType, IFile::AccessMethod::Enum eAccess) const
{
	if (SysString::INVALID_HASH == GetHash())
	{
		return false;
	}

	if (Type::Any != eType)
	{
		if (GetFileType() != eType)
		{
			return false;
		}
	}

	if (AccessMethod::Any != eAccess)
	{
		if (GetAccessMethod() != eAccess)
		{
			return false;
		}
	}

	return true;
}


//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//

