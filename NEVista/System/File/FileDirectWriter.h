#ifndef _FILEDIRECTWRITER_H_
#define _FILEDIRECTWRITER_H_
#pragma once

//----------------------------------------------------------//
// FILEDIRECTWRITER.H
//----------------------------------------------------------//
//-- Description
// CFileDirectWriter derives from CFile, and will be 
// sub-classed into
// CFileDirectTextWriter and
// CFileDirectBinaryWriter.
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


class CFileDirectWriter : public CFile
{
	public:

		CFileDirectWriter(const s8* strFileName, Type::Enum eType);
		CFileDirectWriter(const IFixedString& strFileName, Type::Enum eType);
		virtual ~CFileDirectWriter();
	
		// IFile
		virtual bool				IsOpen(void) const;
		// ~IFile

		size_t						Write(size_t nRequestedSize, const s8* pDstBuffer, size_t nDstBufferSize);

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

#endif //_FILEDIRECTWRITER_H_