//----------------------------------------------------------//
// FILEDIRECTWRITER.CPP
//----------------------------------------------------------//
//-- Description
// CFileAccessorDirectWriter utility class. Derives from 
// CFileAccessor class, and will be sub-classed into 
// CFileAccessorDirectTextWriter and 
// CFileAccessorDirectBinaryWriter.
//
// CFileProcessorDirectWriter utility class. Derives from 
// CFileProcessor class, and will be sub-classed into 
// CFileProcessorDirectTextWriter and 
// CFileProcessorDirectBinaryWriter.
//----------------------------------------------------------//


#include "../SystemIncludes.h"
#include "FileDirectWriter.h"


//----------------------------------------------------------//
// DEFINES
//----------------------------------------------------------//

//----------------------------------------------------------//
// GLOBALS
//----------------------------------------------------------//


//----------------------------------------------------------//
// CFileDirectWriter::CFileDirectWriter
//----------------------------------------------------------//
CFileAccessorDirectWriter::CFileAccessorDirectWriter(CFileData* pData)
: CFileAccessor(pData)
{
}


//----------------------------------------------------------//
// CFileAccessorDirectWriter::~CFileAccessorDirectWriter
//----------------------------------------------------------//
CFileAccessorDirectWriter::~CFileAccessorDirectWriter()
{
}


//----------------------------------------------------------//
// CFileAccessorDirectWriter::IsOpen
//----------------------------------------------------------//
//-- Description
// Returns whether file is open or not.
//----------------------------------------------------------//
bool CFileAccessorDirectWriter::IsOpen(void) const
{
	if (IS_TRUE(ValidateData()))
	{
		return (FileIO::INVALID_HANDLE != m_pData->m_DirectWriterData.m_pFile);
	}

	//-- Not open
	return false;
}


//----------------------------------------------------------//
// CFileAccessorDirectWriter::Write
//----------------------------------------------------------//
//-- Description
// Read bytes from an open direct access file.
// Note that reading '\r\n' from a text file may be converted
// to only '\n' in the buffer on some systems.
//----------------------------------------------------------//
size_t CFileAccessorDirectWriter::Write(size_t nRequestedSize, s8* pSrcBuffer, size_t nSrcBufferSize)
{
	if ( IS_TRUE(ValidateData())
		&& IS_TRUE(IsOpen()) 
		&& IS_PTR(pSrcBuffer)
		&& (nSrcBufferSize >= nRequestedSize) )
	{
		size_t nBytes = FileIO::Fwrite(m_pData->m_DirectWriterData.m_pFile, 1, nRequestedSize, pSrcBuffer, nSrcBufferSize);
		FileIO::Fflush(m_pData->m_DirectWriterData.m_pFile);
		return nBytes;
	}

	return 0;
}


//----------------------------------------------------------//
// CFileProcessorDirectWriter::CFileProcessorDirectWriter
//----------------------------------------------------------//
CFileProcessorDirectWriter::CFileProcessorDirectWriter(CFileData* pData)
: CFileProcessor(pData)
{
}


//----------------------------------------------------------//
// CFileProcessorDirectWriter::~CFileProcessorDirectWriter
//----------------------------------------------------------//
CFileProcessorDirectWriter::~CFileProcessorDirectWriter()
{
}


//----------------------------------------------------------//
// CFileProcessorDirectWriter::IsOpen
//----------------------------------------------------------//
//-- Description
// Returns whether file is open or not.
//----------------------------------------------------------//
bool CFileProcessorDirectWriter::IsOpen(void) const
{
	if (IS_TRUE(ValidateData()))
	{
		return (FileIO::INVALID_HANDLE != m_pData->m_DirectWriterData.m_pFile);
	}

	//-- Not open
	return false;
}


CFileProcessor::Error::Enum CFileProcessorDirectWriter::Open(void)
{
	if (IS_TRUE(ValidateData()))
	{
		m_pData->m_DirectWriterData.m_pFile = FileIO::INVALID_HANDLE;
		m_pData->m_DirectWriterData.m_nWritten = 0;

		return Error::Ok;
	}

	return Error::Failed;
}


CFileProcessor::Error::Enum CFileProcessorDirectWriter::Close(void)
{
	if (IS_TRUE(ValidateData()))
	{
		if (IS_TRUE(IsOpen()))
		{
			FileIO::Fclose(m_pData->m_DirectWriterData.m_pFile);
		}
		m_pData->m_DirectWriterData.m_nWritten = 0;

		return Error::Ok;
	}

	return Error::Failed;
}


CFileProcessor::Error::Enum CFileProcessorDirectWriter::Update(void)
{
	return Error::Ok;
}


//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//

