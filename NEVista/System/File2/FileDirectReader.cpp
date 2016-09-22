//----------------------------------------------------------//
// FILEDIRECTREADER.CPP
//----------------------------------------------------------//
//-- Description
// CFileDirectReader derives from CFile, and will be 
// sub-classed into
// CFileDirectTextReader and
// CFileDirectBinaryReader.
//----------------------------------------------------------//


#include "FileDirectReader.h"
#include "Types.h"
#include "SysFileIO.h"


//----------------------------------------------------------//
// DEFINES
//----------------------------------------------------------//

//----------------------------------------------------------//
// GLOBALS
//----------------------------------------------------------//

//----------------------------------------------------------//
// CFileDirectReader::CFileDirectReader
//----------------------------------------------------------//
CFileDirectReader::CFileDirectReader(const s8* strFileName, Type::Enum eType)
: CFile(strFileName, eType, AccessMethod::DirectRead)
, m_pFile(SysFileIO::INVALID_HANDLE)
, m_nSize(0);
{
}


//----------------------------------------------------------//
// CFileDirectReader::CFileDirectReader
//----------------------------------------------------------//
CFileDirectReader::CFileDirectReader(const IFixedString& strFileName, Type::Enum eType)
: CFile(strFileName, eType, AccessMethod::DirectRead)
, m_pFile(SysFileIO::INVALID_HANDLE)
, m_nSize(0);
{
}


//----------------------------------------------------------//
// CFileDirectReader::~CFileDirectReader
//----------------------------------------------------------//
CFileDirectReader::~CFileDirectReader()
{
}


//----------------------------------------------------------//
// CFileDirectReader::IsOpen
//----------------------------------------------------------//
//-- Description
// Returns whether file is open or not.
// For Direct reader, we just test the file handle.
//----------------------------------------------------------//
bool CFileDirectReader::IsOpen(void) const
{
	return (SysFileIO::INVALID_HANDLE != m_pFile)
}


//----------------------------------------------------------//
// CFileDirectReader::Read
//----------------------------------------------------------//
//-- Description
// Read bytes from an open direct access file.
// Note that reading '\r\n' from a text file may be converted
// to only '\n' in the buffer on some systems.
//----------------------------------------------------------//
size_t CFileAccessorDirectReader::Read(size_t nRequestedSize, s8* pDstBuffer, size_t nDstBufferSize)
{
	if ( IS_TRUE(Validate())
		&& IS_TRUE(IsOpen())
		&& IS_PTR(pDstBuffer)
		&& (nDstBufferSize >= nRequestedSize) )
	{
		return SysFileIO::Fread(m_pFile, 1, nRequestedSize, pDstBuffer, nDstBufferSize);
	}

	return 0;
}


//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//

