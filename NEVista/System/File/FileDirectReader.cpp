//----------------------------------------------------------//
// FILEDIRECTREADER.CPP
//----------------------------------------------------------//
//-- Description
// CFileAccessorDirectReader utility class. Derives from 
// CFileAccessor class, and will be sub-classed into 
// CFileAccessorDirectTextReader and
// CFileAccessorDirectBinaryReader.
//
// CFileProcessorDirectReader utility class. Derives from 
// CFileProcessor class, and will be sub-classed into 
// CFileProcessorDirectTextReader and
// CFileProcessorDirectBinaryReader.
//----------------------------------------------------------//


#include "../SystemIncludes.h"
#include "FileDirectReader.h"


//----------------------------------------------------------//
// DEFINES
//----------------------------------------------------------//

//----------------------------------------------------------//
// GLOBALS
//----------------------------------------------------------//


//----------------------------------------------------------//
// CFileAccessorDirectReader::CFileAccessorDirectReader
//----------------------------------------------------------//
CFileAccessorDirectReader::CFileAccessorDirectReader(CFileData* pData)
: CFileAccessor(pData)
{
}


//----------------------------------------------------------//
// CFileAccessorDirectReader::~CFileAccessorDirectReader
//----------------------------------------------------------//
CFileAccessorDirectReader::~CFileAccessorDirectReader()
{
}


//----------------------------------------------------------//
// CFileAccessorDirectReader::IsOpen
//----------------------------------------------------------//
//-- Description
// Returns whether file is open or not.
//----------------------------------------------------------//
bool CFileAccessorDirectReader::IsOpen(void) const
{
	if (IS_TRUE(ValidateData()))
	{
		return (FileIO::INVALID_HANDLE != m_pData->m_DirectReaderData.m_pFile);
	}

	//-- Not open
	return false;
}


//----------------------------------------------------------//
// CFileAccessorDirectReader::Read
//----------------------------------------------------------//
//-- Description
// Read bytes from an open direct access file.
// Note that reading '\r\n' from a text file may be converted
// to only '\n' in the buffer on some systems.
//----------------------------------------------------------//
size_t CFileAccessorDirectReader::Read(size_t nRequestedSize, s8* pDstBuffer, size_t nDstBufferSize)
{
	if ( IS_TRUE(ValidateData())
		&& IS_TRUE(IsOpen())
		&& IS_PTR(pDstBuffer)
		&& (nDstBufferSize >= nRequestedSize) )
	{
		return FileIO::Fread(m_pData->m_DirectReaderData.m_pFile, 1, nRequestedSize, pDstBuffer, nDstBufferSize);
	}

	return 0;
}


//----------------------------------------------------------//
// CFileProcessorDirectReader::CFileProcessorDirectReader
//----------------------------------------------------------//
CFileProcessorDirectReader::CFileProcessorDirectReader(CFileData* pData)
: CFileProcessor(pData)
{
}


//----------------------------------------------------------//
// CFileProcessorDirectReader::~CFileProcessorDirectReader
//----------------------------------------------------------//
CFileProcessorDirectReader::~CFileProcessorDirectReader()
{
}


//----------------------------------------------------------//
// CFileProcessorDirectReader::IsOpen
//----------------------------------------------------------//
//-- Description
// Returns whether file is open or not.
//----------------------------------------------------------//
bool CFileProcessorDirectReader::IsOpen(void) const
{
	if (IS_TRUE(ValidateData()))
	{
		return (FileIO::INVALID_HANDLE != m_pData->m_DirectReaderData.m_pFile);
	}

	//-- Not open
	return false;
}


CFileProcessor::Error::Enum CFileProcessorDirectReader::Open(void)
{
	if (IS_TRUE(ValidateData()))
	{
		m_pData->m_DirectReaderData.m_pFile = FileIO::INVALID_HANDLE;
		m_pData->m_DirectReaderData.m_nSize = 0;

		return Error::Ok;
	}

	return Error::Failed;
}


CFileProcessor::Error::Enum CFileProcessorDirectReader::Close(void)
{
	if (IS_TRUE(ValidateData()))
	{
		if (IS_TRUE(IsOpen()))
		{
			FileIO::Fclose(m_pData->m_DirectReaderData.m_pFile);
		}
		m_pData->m_DirectReaderData.m_nSize = 0;

		return Error::Ok;
	}

	return Error::Failed;
}


CFileProcessor::Error::Enum CFileProcessorDirectReader::Update(void)
{
	return Error::Ok;
}


//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//

