//----------------------------------------------------------//
// FILEDIRECTBINARYWRITER.CPP
//----------------------------------------------------------//
//-- Description
// CFileDirectBinaryWriter class. Derived from
// CFileDirectWriter.
//----------------------------------------------------------//


#include "FileDirectBinaryWriter.h"
#include "Types.h"
#include "SysFileIO.h"
#include "SysString.h"


//----------------------------------------------------------//
// DEFINES
//----------------------------------------------------------//

//----------------------------------------------------------//
// GLOBALS
//----------------------------------------------------------//

//----------------------------------------------------------//
// CFileDirectBinaryWriter::CFileDirectBinaryWriter
//----------------------------------------------------------//
CFileDirectBinaryWriter::CFileDirectBinaryWriter(const s8* strFileName)
: CFileDirectWriter(strFileName, Type::Binary)
{
}


//----------------------------------------------------------//
// CFileDirectBinaryWriter::CFileDirectBinaryWriter
//----------------------------------------------------------//
CFileDirectBinaryWriter::CFileDirectBinaryWriter(const IFixedString& strFileName)
: CFileDirectWriter(strFileName, Type::Binary)
{
}


//----------------------------------------------------------//
// CFileDirectBinaryWriter::~CFileDirectBinaryWriter
//----------------------------------------------------------//
CFileDirectBinaryWriter::~CFileDirectBinaryWriter()
{
}


//----------------------------------------------------------//
// CFileDirectBinaryWriter::Validate
//----------------------------------------------------------//
bool CFileDirectBinaryWriter::Validate(void) const
{
	return IsTypeAccess(Type::Binary, AccessMethod::DirectWrite);
}


//----------------------------------------------------------//
// CFileDirectBinaryWriter::Open
//----------------------------------------------------------//
CFile::Error::Enum CFileDirectBinaryWriter::Open(void)
{
	if (IS_TRUE(Validate()))
	{
		if (IS_FALSE(IsOpen()))
		{
			m_pFile = SysFileIO::Fopen(m_strFileName.ConstBuffer(), "wb");
			m_nSize = 0;

			if (IS_TRUE(IsOpen()))
			{
				return Error::Ok;
			}

			return Error::FileOpenFailed;
		}

		return Error::FileAlreadyOpen;
	}

	return Error::Failed;
}


//----------------------------------------------------------//
// CFileDirectBinaryWriter::Close
//----------------------------------------------------------//
CFile::Error::Enum CFileDirectBinaryWriter::Close(void)
{
	if (IS_TRUE(IsOpen()))
	{
		SysFileIO::Fclose(m_pFile);
	}

	m_pFile = SysFileIO::INVALID_HANDLE;
	m_nSize = 0;

	return Error::Ok;
}


//----------------------------------------------------------//
// CFileDirectBinaryWriter::Update
//----------------------------------------------------------//
CFile::Error::Enum CFileDirectBinaryWriter::Update(void)
{
	return Error::Ok;
}


//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//

