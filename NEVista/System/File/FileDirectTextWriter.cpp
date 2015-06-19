//----------------------------------------------------------//
// FILEDIRECTTEXTWRITER.CPP
//----------------------------------------------------------//
//-- Description
// CFileAccessorDirectTextWriter class. Derived from
// CFileAccessorDirectWriter.
//
// CFileProcessorDirectTextWriter class. Derived from
// CFileProcessorDirectWriter.
//----------------------------------------------------------//


#include "../SystemIncludes.h"
#include "FileDirectTextWriter.h"


//----------------------------------------------------------//
// DEFINES
//----------------------------------------------------------//

//----------------------------------------------------------//
// GLOBALS
//----------------------------------------------------------//


//----------------------------------------------------------//
// CFileAccessorDirectTextWriter::CFileAccessorDirectTextWriter
//----------------------------------------------------------//
CFileAccessorDirectTextWriter::CFileAccessorDirectTextWriter(CFileData* pData)
: CFileAccessorDirectWriter(pData)
{
}


//----------------------------------------------------------//
// CFileAccessorDirectTextWriter::~CFileAccessorDirectTextWriter
//----------------------------------------------------------//
CFileAccessorDirectTextWriter::~CFileAccessorDirectTextWriter()
{
}


//----------------------------------------------------------//
// CFileAccessorDirectTextWriter::ValidateData
//----------------------------------------------------------//
bool CFileAccessorDirectTextWriter::ValidateData(void) const
{
	assert(m_pData);
	assert(IS_TRUE(m_pData->Validate(CFileData::Type::Text, CFileData::AccessMethod::DirectWrite)));
	
	if (m_pData)
	{
		if (IS_TRUE(m_pData->Validate(CFileData::Type::Text, CFileData::AccessMethod::DirectWrite)))
		{
			//-- Data validated
			return true;
		}
	}

	//-- Failed to validate data
	return false;
}


//----------------------------------------------------------//
// CFileAccessorDirectTextWriter::Printf
//----------------------------------------------------------//
//-- Description
// Uses printf-style formatting to put a string directly into
// an open file.
//----------------------------------------------------------//
size_t CFileAccessorDirectTextWriter::Printf(const s8* strFormatting, ...)
{
	m_strWorkingBuffer.Clear();

	va_list ArgList;
	va_start(ArgList, strFormatting);
	String::Vsprintf(m_strWorkingBuffer.Buffer(), FILE_WORKING_TEXT_BUFFER_SIZE, strFormatting, ArgList);
	va_end(ArgList);

	return PutString(m_strWorkingBuffer.Buffer());
}


//----------------------------------------------------------//
// CFileAccessorDirectTextWriter::PutString
//----------------------------------------------------------//
//-- Description 
// Attempts to put a complete string into an open file.
//----------------------------------------------------------//
size_t CFileAccessorDirectTextWriter::PutString(const s8* strSrcBuffer)
{
	if ( IS_TRUE(ValidateData())
		&& IS_TRUE(IsOpen())
		&& strSrcBuffer )
	{
		size_t nBytes = FileIO::Fputs(m_pData->m_DirectWriterData.m_pFile, strSrcBuffer);
		FileIO::Fflush(m_pData->m_DirectWriterData.m_pFile);
		return nBytes;
	}

	return 0;
}



//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//

