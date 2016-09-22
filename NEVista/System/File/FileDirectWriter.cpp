//----------------------------------------------------------//
// FILEDIRECTWRITER.CPP
//----------------------------------------------------------//
//-- Description
// CFileDirectWriter derives from CFile, and will be 
// sub-classed into
// CFileDirectTextWriter and
// CFileDirectBinaryWriter.
//----------------------------------------------------------//


#include "FileDirectWriter.h"
#include "Types.h"
#include "SysFileIO.h"


//----------------------------------------------------------//
// DEFINES
//----------------------------------------------------------//

//----------------------------------------------------------//
// GLOBALS
//----------------------------------------------------------//

//----------------------------------------------------------//
// CFileDirectWriter::CFileDirectWriter
//----------------------------------------------------------//
CFileDirectWriter::CFileDirectWriter(const s8* strFileName, Type::Enum eType)
: CFile(strFileName, eType, AccessMethod::DirectWrite)
, m_pFile(SysFileIO::INVALID_HANDLE)
, m_nSize(0)
{
}


//----------------------------------------------------------//
// CFileDirectWriter::CFileDirectWriter
//----------------------------------------------------------//
CFileDirectWriter::CFileDirectWriter(const IFixedString& strFileName, Type::Enum eType)
: CFile(strFileName, eType, AccessMethod::DirectWrite)
, m_pFile(SysFileIO::INVALID_HANDLE)
, m_nSize(0)
{
}


//----------------------------------------------------------//
// CFileDirectWriter::~CFileDirectWriter
//----------------------------------------------------------//
CFileDirectWriter::~CFileDirectWriter()
{
}


//----------------------------------------------------------//
// CFileDirectWriter::IsOpen
//----------------------------------------------------------//
//-- Description
// Returns whether file is open or not.
// For Direct writer, we just test the file handle.
//----------------------------------------------------------//
bool CFileDirectWriter::IsOpen(void) const
{
	return (SysFileIO::INVALID_HANDLE != m_pFile);
}


//----------------------------------------------------------//
// CFileDirectWriter::Writer
//----------------------------------------------------------//
//-- Description
// Write bytes to an open direct access file.
//----------------------------------------------------------//
size_t CFileDirectWriter::Write(size_t nRequestedSize, const s8* pSrcBuffer, size_t nSrcBufferSize)
{
	if ( IS_TRUE(Validate())
		&& IS_TRUE(IsOpen())
		&& IS_PTR(pSrcBuffer)
		&& (nSrcBufferSize >= nRequestedSize) )
	{
		size_t nBytes = SysFileIO::Fwrite(m_pFile, 1, nRequestedSize, pSrcBuffer, nSrcBufferSize);
		SysFileIO::Fflush(m_pFile);
		return nBytes;
	}

	return 0;
}


//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//

