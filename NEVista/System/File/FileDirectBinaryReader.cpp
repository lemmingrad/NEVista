//----------------------------------------------------------//
// FILEDIRECTBINARYREADER.CPP
//----------------------------------------------------------//
//-- Description
// CFileDirectBinaryReader class. Derived from
// CFileDirectReader.
//----------------------------------------------------------//


#include "FileDirectBinaryReader.h"
#include "Types.h"
#include "SysFileIO.h"


//----------------------------------------------------------//
// DEFINES
//----------------------------------------------------------//

//----------------------------------------------------------//
// GLOBALS
//----------------------------------------------------------//


//----------------------------------------------------------//
// CFileDirectBinaryReader::CFileDirectBinaryReader
//----------------------------------------------------------//
CFileDirectBinaryReader::CFileDirectBinaryReader(const s8* strFileName)
: CFileDirectReader(strFileName, Type::Binary)
{
}


//----------------------------------------------------------//
// CFileDirectBinaryReader::CFileDirectBinaryReader
//----------------------------------------------------------//
CFileDirectBinaryReader::CFileDirectBinaryReader(const IFixedString& strFileName)
: CFileDirectReader(strFileName, Type::Binary)
{
}


//----------------------------------------------------------//
// CFileDirectBinaryReader::~CFileDirectBinaryReader
//----------------------------------------------------------//
CFileDirectBinaryReader::~CFileDirectBinaryReader()
{
}


//----------------------------------------------------------//
// CFileDirectBinaryReader::Validate
//----------------------------------------------------------//
bool CFileDirectBinaryReader::Validate(void) const
{
	return IsTypeAccess(Type::Binary, AccessMethod::DirectRead);
}


//----------------------------------------------------------//
// CFileDirectBinaryReader::Open
//----------------------------------------------------------//
CFile::Error::Enum CFileDirectBinaryReader::Open(void)
{
	if (IS_TRUE(Validate()))
	{
		if (IS_FALSE(IsOpen()))
		{
			m_pFile = SysFileIO::Fopen(m_strFileName.ConstBuffer(), "rb");
			m_nSize = 0;

			if (IS_TRUE(IsOpen()))
			{
				SysFileIO::Fseek(m_pFile, 0, SEEK_END);
				m_nSize = SysFileIO::Ftell(m_pFile);
				SysFileIO::Fseek(m_pFile, 0, SEEK_SET);

				return Error::Ok;
			}

			return Error::FileOpenFailed;
		}

		return Error::FileAlreadyOpen;
	}

	return Error::Failed;
}


//----------------------------------------------------------//
// CFileDirectBinaryReader::Close
//----------------------------------------------------------//
CFile::Error::Enum CFileDirectBinaryReader::Close(void)
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
// CFileDirectBinaryReader::Update
//----------------------------------------------------------//
CFile::Error::Enum CFileDirectBinaryReader::Update(void)
{
	return Error::Ok;
}


//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//

