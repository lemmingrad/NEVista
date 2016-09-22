#ifndef _FILEDIRECTREADER_H_
#define _FILEDIRECTREADER_H_
#pragma once

//----------------------------------------------------------//
// FILEDIRECTREADER.H
//----------------------------------------------------------//
//-- Description
// CFileDirectReader derives from CFile, and will be 
// sub-classed into
// CFileDirectTextReader and
// CFileDirectBinaryReader.
//----------------------------------------------------------//


#include "File.h"
#include "Types.h"
#include "SysFileIO.h"


//----------------------------------------------------------//
// DEFINES
//----------------------------------------------------------//

//----------------------------------------------------------//
// ENUMS
//----------------------------------------------------------//

//----------------------------------------------------------//
// STRUCTS
//----------------------------------------------------------//

//----------------------------------------------------------//
// FORWARD REFERENCES
//----------------------------------------------------------//

//----------------------------------------------------------//
// CLASSES
//----------------------------------------------------------//


class CFileDirectReader : public CFile
{
	public:

		CFileDirectReader(const s8* strFileName, Type::Enum eType);
		CFileDirectReader(const IFixedString& strFileName, Type::Enum eType);
		virtual ~CFileDirectReader();
	
		// IFile
		virtual bool				IsOpen(void) const;
		// ~IFile

		size_t						Read(size_t nRequestedSize, s8* pDstBuffer, size_t nDstBufferSize);

	protected:

		SysFileIO::Handle			m_pFile;
		size_t						m_nSize;
};


//----------------------------------------------------------//
// EXTERNALS
//----------------------------------------------------------//

//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//

#endif //_FILEDIRECTREADER_H_