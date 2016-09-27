#ifndef _FILEBUFFEREDREADER_H_
#define _FILEBUFFEREDREADER_H_
#pragma once

//----------------------------------------------------------//
// FILEBUFFEREDREADER.H
//----------------------------------------------------------//
//-- Description
// CFileBufferedReader derives from CFile, and will be 
// sub-classed into
// CFileBufferedTextReader and
// CFileBufferedBinaryReader.
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


class CFileBufferedReader : public CFile
{
	public:

		CFileBufferedReader(const s8* strFileName, Type::Enum eType);
		CFileBufferedReader(const IFixedString& strFileName, Type::Enum eType);
		virtual ~CFileBufferedReader();
	
		// IFile
		virtual bool				IsOpen(void) const;
		// ~IFile

		bool						IsBufferAttached(void) const;
		size_t						Read(size_t nRequestedSize, s8* pDstBuffer, size_t nDstBufferSize);
		void						Flush(void);

	protected:

		ISimpleBuffer*				m_pBuffer;
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