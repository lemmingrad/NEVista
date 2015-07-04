#ifndef _FILEDIRECTREADER_H_
#define _FILEDIRECTREADER_H_
#pragma once

//----------------------------------------------------------//
// FILEDIRECTREADER.H
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


#include "Types.h"
#include "FileAccessor.h"
#include "FileProcessor.h"


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


class CFileData;


//----------------------------------------------------------//
// CLASSES
//----------------------------------------------------------//


class CFileAccessorDirectReader : public CFileAccessor
{
	public:

		CFileAccessorDirectReader(CFileData* pData);
		virtual ~CFileAccessorDirectReader();

		virtual bool				ValidateData(void) const = 0;
		virtual bool				IsOpen(void) const;

		size_t						Read(size_t nRequestedSize, s8* pDstBuffer, size_t nDstBufferSize);
};


class CFileProcessorDirectReader : public CFileProcessor
{
	public:

		CFileProcessorDirectReader(CFileData* pData);
		virtual ~CFileProcessorDirectReader();

		virtual bool				ValidateData(void) const = 0;
		virtual bool				IsOpen(void) const;

		virtual Error::Enum			Open(void);
		virtual Error::Enum			Close(void);
		virtual Error::Enum			Update(void);
};


//----------------------------------------------------------//
// EXTERNALS
//----------------------------------------------------------//

//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//

#endif //_FILEDIRECTREADER_H_