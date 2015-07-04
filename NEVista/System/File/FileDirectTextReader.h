#ifndef _FILEDIRECTTEXTREADER_H_
#define _FILEDIRECTTEXTREADER_H_
#pragma once

//----------------------------------------------------------//
// FILEDIRECTTEXTREADER.H
//----------------------------------------------------------//
//-- Description
// CFileAccessorDirectTextReader class. Derived from
// CFileAccessorDirectReader.
//
// CFileProcessorDirectTextReader class. Derived from
// CFileProcessorDirectReader.
//----------------------------------------------------------//


#include "Types.h"
#include "FileDirectReader.h"


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


class CFileAccessorDirectTextReader : public CFileAccessorDirectReader
{
	public:

		CFileAccessorDirectTextReader(CFileData* pData);
		virtual ~CFileAccessorDirectTextReader();

		virtual bool							ValidateData(void) const;

		s8*										GetString(s8* pDstBuffer, size_t nDstBufferSize);
};


class CFileProcessorDirectTextReader : public CFileProcessorDirectReader
{
	public:

		CFileProcessorDirectTextReader(CFileData* pData);
		virtual ~CFileProcessorDirectTextReader();

		virtual bool							ValidateData(void) const;

		virtual Error::Enum						Open(void);
		virtual Error::Enum						Close(void);
		virtual Error::Enum						Update(void);
};


//----------------------------------------------------------//
// EXTERNALS
//----------------------------------------------------------//

//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//

#endif //_FILEDIRECTTEXTREADER_H_