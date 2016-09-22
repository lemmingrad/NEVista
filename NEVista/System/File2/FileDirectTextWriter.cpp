//----------------------------------------------------------//
// FILEDIRECTTEXTREADER.CPP
//----------------------------------------------------------//
//-- Description
// CFileDirectTextReader class. Derived from
// CFileDirectReader.
//----------------------------------------------------------//


#include "FileDirectTextReader.h"
#include "Types.h"
#include "SysFileIO.h"


//----------------------------------------------------------//
// DEFINES
//----------------------------------------------------------//

//----------------------------------------------------------//
// GLOBALS
//----------------------------------------------------------//


//----------------------------------------------------------//
// CFileDirectTextReader::CFileDirectTextReader
//----------------------------------------------------------//
CFileDirectTextReader::CFileDirectTextReader(const s8* strFileName)
: CFileDirectReader(strFileName, Type::Text)
{
}


//----------------------------------------------------------//
// CFileDirectTextReader::CFileDirectTextReader
//----------------------------------------------------------//
CFileDirectTextReader::CFileDirectTextReader(const IFixedString& strFileName)
: CFileDirectReader(strFileName, Type::Text)
{
}


//----------------------------------------------------------//
// CFileDirectTextReader::~CFileDirectTextReader
//----------------------------------------------------------//
CFileDirectTextReader::~CFileDirectTextReader()
{
}


//----------------------------------------------------------//
// CFileDirectTextReader::Validate
//----------------------------------------------------------//
bool CFileDirectTextReader::Validate(void) const
{
	return IsTypeAccess(Type::Text, AccessMethod::DirectRead);
}


//----------------------------------------------------------//
// CFileDirectTextReader::GetString
//----------------------------------------------------------//
//-- Description
// Read a string from an open direct access file.
// A string is determined as terminated by a newline
// character or the end of file. Any trailing newline will be
// stripped from the output string.
//----------------------------------------------------------//
s8* CFileDirectTextReader::GetString(s8* pDstBuffer, size_t nDstBufferSize)
{
	if ( IS_TRUE(Validate())
		&& IS_TRUE(IsOpen())
		&& IS_PTR(pDstBuffer)
		&& (nDstBufferSize > 0) )
	{
		return SysFileIO::Fgets(m_pFile, pDstBuffer, nDstBufferSize);
	}

	return NULL;
}


CFile::Error::Enum CFileDirectTextReader::Open(void)
{
	if (IS_TRUE(Validate()))
	{
		if (IS_FALSE(IsOpen()))
		{
			m_pFile = SysFileIO::Fopen(m_pData->m_strFileName.ConstBuffer(), "rt");
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


CFile::Error::Enum CFileDirectTextReader::Close(void)
{
	if (IS_TRUE(IsOpen()))
	{
		SysFileIO::Fclose(m_pFile);
	}

	m_pFile = SysFileIO::INVALID_HANDLE;
	m_nSize = 0;

	return Error::Ok;
}


CFile::Error::Enum CFileDirectTextReader::Update(void)
{
	return Error::Ok;
}


//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//

