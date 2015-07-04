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


#include "FileDirectTextWriter.h"
#include "FileData.h"
#include "Types.h"
#include "SysString.h"
#include "SysFileIO.h"


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
	SysString::Vsprintf(m_strWorkingBuffer.Buffer(), m_strWorkingBuffer.Size(), strFormatting, ArgList);
	va_end(ArgList);

	return PutString(m_strWorkingBuffer.ConstBuffer());
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
		size_t nBytes = SysFileIO::Fputs(m_pData->m_DirectWriterData.m_pFile, strSrcBuffer);
		SysFileIO::Fflush(m_pData->m_DirectWriterData.m_pFile);
		return nBytes;
	}

	return 0;
}


//----------------------------------------------------------//
// CFileProcessorDirectTextWriter::CFileProcessorDirectTextWriter
//----------------------------------------------------------//
CFileProcessorDirectTextWriter::CFileProcessorDirectTextWriter(CFileData* pData)
: CFileProcessorDirectWriter(pData)
{
}


//----------------------------------------------------------//
// CFileProcessorDirectTextWriter::~CFileProcessorDirectTextWriter
//----------------------------------------------------------//
CFileProcessorDirectTextWriter::~CFileProcessorDirectTextWriter()
{
}


//----------------------------------------------------------//
// CFileProcessorDirectTextWriter::ValidateData
//----------------------------------------------------------//
bool CFileProcessorDirectTextWriter::ValidateData(void) const
{
	assert(IS_PTR(m_pData));
	assert(IS_TRUE(m_pData->Validate(CFileData::Type::Text, CFileData::AccessMethod::DirectWrite)));
	
	if (IS_PTR(m_pData))
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


CFileProcessor::Error::Enum CFileProcessorDirectTextWriter::Open(void)
{
	if (IS_TRUE(ValidateData()))
	{
		Error::Enum eResult = CFileProcessorDirectWriter::Open();
		if (Error::Ok == eResult)
		{
			m_pData->m_DirectWriterData.m_pFile = SysFileIO::Fopen(m_pData->m_strFileName.ConstBuffer(), "wt");
			if (IS_FALSE(IsOpen()))
			{
				//-- Failed to open file
				return Error::Failed;
			}

			return Error::Ok;
		}
	}

	return Error::Failed;
}


CFileProcessor::Error::Enum CFileProcessorDirectTextWriter::Close(void)
{
	return CFileProcessorDirectWriter::Close();
}


CFileProcessor::Error::Enum CFileProcessorDirectTextWriter::Update(void)
{
	return CFileProcessorDirectWriter::Update();
}


//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//

