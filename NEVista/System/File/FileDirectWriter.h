#ifndef _FILEDIRECTWRITER_H_
#define _FILEDIRECTWRITER_H_
#pragma once

//----------------------------------------------------------//
// FILEDIRECTWRITER.H
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


#include "FileData.h"
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
// CLASSES
//----------------------------------------------------------//


class CFileAccessorDirectWriter : public CFileAccessor
{
	public:

		CFileAccessorDirectWriter(CFileData* pData);
		virtual ~CFileAccessorDirectWriter();

		virtual bool				ValidateData(void) const = 0;
		virtual bool				IsOpen(void) const;

		size_t						Write(size_t nRequestedSize, s8* pSrcBuffer, size_t nSrcBufferSize);
};


class CFileProcessorDirectWriter : public CFileProcessor
{
	public:

		CFileProcessorDirectWriter(CFileData* pData);
		virtual ~CFileProcessorDirectWriter();

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

#endif //_FILEDIRECTWRITER_H_