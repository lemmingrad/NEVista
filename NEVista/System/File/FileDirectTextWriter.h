#ifndef _FILEDIRECTTEXTWRITER_H_
#define _FILEDIRECTTEXTWRITER_H_
#pragma once

//----------------------------------------------------------//
// FILEDIRECTTEXTWRITER.H
//----------------------------------------------------------//
//-- Description
// CFileAccessorDirectTextWriter utility class. Derives from 
// CFileAccessorDirectWriter class.
//
// CFileProcessorDirectTextWriter utility class. Derives from 
// CFileProcessorDirectWriter class.
//----------------------------------------------------------//


#include "FileData.h"
#include "FileDirectWriter.h"


//----------------------------------------------------------//
// DEFINES
//----------------------------------------------------------//


#define FILE_WORKING_TEXT_BUFFER_SIZE				(2048)


//----------------------------------------------------------//
// ENUMS
//----------------------------------------------------------//

//----------------------------------------------------------//
// STRUCTS
//----------------------------------------------------------//

//----------------------------------------------------------//
// CLASSES
//----------------------------------------------------------//


class CFileAccessorDirectTextWriter : public CFileAccessorDirectWriter
{
	protected:

		FixedString<FILE_WORKING_TEXT_BUFFER_SIZE> m_strWorkingBuffer;

	public:

		CFileAccessorDirectTextWriter(CFileData* pData);
		virtual ~CFileAccessorDirectTextWriter();

		virtual bool							ValidateData(void) const;

		size_t									Printf(const s8* strFormatting, ...);
		size_t									PutString(const s8* pDstBuffer);
};


class CFileProcessorDirectTextWriter : public CFileProcessorDirectWriter
{
	public:

		CFileProcessorDirectTextWriter(CFileData* pData);
		virtual ~CFileProcessorDirectTextWriter();

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

#endif //_FILEDIRECTTEXTWRITER_H_