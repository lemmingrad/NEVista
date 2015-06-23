//----------------------------------------------------------//
// FILEDIRECTTEXTREADER.CPP
//----------------------------------------------------------//
//-- Description
// CFileAccessorDirectTextReader class. Derived from
// CFileAccessorDirectReader.
//
// CFileProcessorDirectTextReader class. Derived from
// CFileProcessorDirectReader.
//----------------------------------------------------------//


#include "../SystemIncludes.h"
#include "FileDirectTextReader.h"


//----------------------------------------------------------//
// DEFINES
//----------------------------------------------------------//

//----------------------------------------------------------//
// GLOBALS
//----------------------------------------------------------//


//----------------------------------------------------------//
// CFileAccessorDirectTextReader::CFileAccessorDirectTextReader
//----------------------------------------------------------//
CFileAccessorDirectTextReader::CFileAccessorDirectTextReader(CFileData* pData)
: CFileAccessorDirectReader(pData)
{
}


//----------------------------------------------------------//
// CFileAccessorDirectTextReader::~CFileAccessorDirectTextReader
//----------------------------------------------------------//
CFileAccessorDirectTextReader::~CFileAccessorDirectTextReader()
{
}


//----------------------------------------------------------//
// CFileAccessorDirectTextReader::ValidateData
//----------------------------------------------------------//
bool CFileAccessorDirectTextReader::ValidateData(void) const
{
	assert(IS_PTR(m_pData));
	assert(IS_TRUE(m_pData->Validate(CFileData::Type::Text, CFileData::AccessMethod::DirectRead)));
	
	if (IS_PTR(m_pData))
	{
		if (IS_TRUE(m_pData->Validate(CFileData::Type::Text, CFileData::AccessMethod::DirectRead)))
		{
			//-- Data validated
			return true;
		}
	}

	//-- Failed to validate data
	return false;
}


//----------------------------------------------------------//
// CFileAccessorDirectTextReader::GetString
//----------------------------------------------------------//
//-- Description
// Read a string from an open direct access file.
// A string is determined as terminated by a newline
// character or the end of file. Any trailing newline will be
// stripped from the output string.
//----------------------------------------------------------//
s8* CFileAccessorDirectTextReader::GetString(s8* pDstBuffer, size_t nDstBufferSize)
{
	if ( IS_TRUE(ValidateData())
		&& IS_TRUE(IsOpen())
		&& IS_PTR(pDstBuffer)
		&& (nDstBufferSize > 0) )
	{
		return FileIO::Fgets(m_pData->m_DirectReaderData.m_pFile, pDstBuffer, nDstBufferSize);
	}

	return NULL;
}


//----------------------------------------------------------//
// CFileProcessorDirectTextReader::CFileProcessorDirectTextReader
//----------------------------------------------------------//
CFileProcessorDirectTextReader::CFileProcessorDirectTextReader(CFileData* pData)
: CFileProcessorDirectReader(pData)
{
}


//----------------------------------------------------------//
// CFileProcessorDirectTextReader::~CFileProcessorDirectTextReader
//----------------------------------------------------------//
CFileProcessorDirectTextReader::~CFileProcessorDirectTextReader()
{
}


//----------------------------------------------------------//
// CFileProcessorDirectTextReader::ValidateData
//----------------------------------------------------------//
bool CFileProcessorDirectTextReader::ValidateData(void) const
{
	assert(IS_PTR(m_pData));
	assert(IS_TRUE(m_pData->Validate(CFileData::Type::Text, CFileData::AccessMethod::DirectRead)));
	
	if (IS_PTR(m_pData))
	{
		if (IS_TRUE(m_pData->Validate(CFileData::Type::Text, CFileData::AccessMethod::DirectRead)))
		{
			//-- Data validated
			return true;
		}
	}

	//-- Failed to validate data
	return false;
}


CFileProcessor::Error::Enum CFileProcessorDirectTextReader::Open(void)
{
	if (IS_TRUE(ValidateData()))
	{
		Error::Enum eResult = CFileProcessorDirectReader::Open();
		if (Error::Ok == eResult)
		{
			m_pData->m_DirectReaderData.m_pFile = FileIO::Fopen(m_pData->m_strFileName.ConstBuffer(), "rt");
			if (IS_FALSE(IsOpen()))
			{
				//-- Failed to open file
				return Error::Failed;
			}

			FileIO::Fseek(m_pData->m_DirectReaderData.m_pFile, 0, SEEK_END);
			m_pData->m_DirectReaderData.m_nSize = FileIO::Ftell(m_pData->m_DirectReaderData.m_pFile);
			FileIO::Fseek(m_pData->m_DirectReaderData.m_pFile, 0, SEEK_SET);

			return Error::Ok;
		}
	}

	return Error::Failed;
}


CFileProcessor::Error::Enum CFileProcessorDirectTextReader::Close(void)
{
	return CFileProcessorDirectReader::Close();
}


CFileProcessor::Error::Enum CFileProcessorDirectTextReader::Update(void)
{
	return CFileProcessorDirectReader::Update();
}


//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//

