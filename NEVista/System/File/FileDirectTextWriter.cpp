//----------------------------------------------------------//
// FILEDIRECTTEXTWRITER.CPP
//----------------------------------------------------------//
//-- Description
// CFileDirectTextWriter class. Derived from
// CFileDirectWriter.
//----------------------------------------------------------//


#include "FileDirectTextWriter.h"
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
// CFileDirectTextWriter::CFileDirectTextWriter
//----------------------------------------------------------//
CFileDirectTextWriter::CFileDirectTextWriter(const s8* strFileName)
: CFileDirectWriter(strFileName, Type::Text)
{
}


//----------------------------------------------------------//
// CFileDirectTextWriter::CFileDirectTextWriter
//----------------------------------------------------------//
CFileDirectTextWriter::CFileDirectTextWriter(const IFixedString& strFileName)
: CFileDirectWriter(strFileName, Type::Text)
{
}


//----------------------------------------------------------//
// CFileDirectTextWriter::~CFileDirectTextWriter
//----------------------------------------------------------//
CFileDirectTextWriter::~CFileDirectTextWriter()
{
}


//----------------------------------------------------------//
// CFileDirectTextWriter::Validate
//----------------------------------------------------------//
bool CFileDirectTextWriter::Validate(void) const
{
	return IsTypeAccess(Type::Text, AccessMethod::DirectWrite);
}


//----------------------------------------------------------//
// CFileDirectTextWriter::Printf
//----------------------------------------------------------//
size_t CFileDirectTextWriter::Printf(const s8* strFormating, ...)
{
	va_list ArgList;
	va_start(ArgList, strFormating);
	size_t nSymbolsConverted = SysString::Vsprintf(m_PrintfBuffer.Buffer(), m_PrintfBuffer.Size(), strFormating, ArgList);
	va_end(ArgList);

	if (nSymbolsConverted > 0)
	{
		return PutString(m_PrintfBuffer);
	}

	return 0;
}
		

//----------------------------------------------------------//
// CFileDirectTextWriter::PutString
//----------------------------------------------------------//
//-- Description
// Write a string into an open file.
//----------------------------------------------------------//
size_t CFileDirectTextWriter::PutString(const s8* pSrcBuffer)
{
	if ( IS_TRUE(Validate())
		&& IS_TRUE(IsOpen())
		&& IS_FALSE(SysString::IsEmpty(pSrcBuffer)) )
	{
		size_t nBytes = SysFileIO::Fputs(m_pFile, pSrcBuffer);
		SysFileIO::Fflush(m_pFile);
		return nBytes;
	}

	return 0;
}


//----------------------------------------------------------//
// CFileDirectTextWriter::PutString
//----------------------------------------------------------//
//-- Description
// Write a string into an open file.
//----------------------------------------------------------//
size_t CFileDirectTextWriter::PutString(const std::string& strString)
{
	if ( IS_TRUE(Validate())
		&& IS_TRUE(IsOpen())
		&& (strString.length() > 0) )
	{
		size_t nBytes = SysFileIO::Fputs(m_pFile, strString.c_str());
		SysFileIO::Fflush(m_pFile);
		return nBytes;
	}

	return 0;
}


//----------------------------------------------------------//
// CFileDirectTextWriter::PutString
//----------------------------------------------------------//
//-- Description
// Write a string into an open file.
//----------------------------------------------------------//
size_t CFileDirectTextWriter::PutString(const IFixedString& strString)
{
	if ( IS_TRUE(Validate())
		&& IS_TRUE(IsOpen())
		&& IS_FALSE(strString.IsEmpty()) )
	{
		size_t nBytes = SysFileIO::Fputs(m_pFile, strString.ConstBuffer());
		SysFileIO::Fflush(m_pFile);
		return nBytes;
	}

	return 0;
}


//----------------------------------------------------------//
// CFileDirectTextWriter::Open
//----------------------------------------------------------//
CFile::Error::Enum CFileDirectTextWriter::Open(void)
{
	if (IS_TRUE(Validate()))
	{
		if (IS_FALSE(IsOpen()))
		{
			m_pFile = SysFileIO::Fopen(m_strFileName.ConstBuffer(), "wt");
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
// CFileDirectTextWriter::Close
//----------------------------------------------------------//
CFile::Error::Enum CFileDirectTextWriter::Close(void)
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
// CFileDirectTextWriter::Update
//----------------------------------------------------------//
CFile::Error::Enum CFileDirectTextWriter::Update(void)
{
	return Error::Ok;
}


//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//

