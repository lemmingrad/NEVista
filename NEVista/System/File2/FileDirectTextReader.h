#ifndef _FILEDIRECTTEXTREADER_H_
#define _FILEDIRECTTEXTREADER_H_
#pragma once

//----------------------------------------------------------//
// FILEDIRECTTEXTREADER.H
//----------------------------------------------------------//
//-- Description
// CFileDirectTextReader class. Derived from
// CFileDirectReader.
//----------------------------------------------------------//


#include "FileDirectReader.h"
#include "Types.h"


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


class CFileDirectTextReader : public CFileDirectReader
{
	public:

		CFileDirectTextReader(const s8* strFileName);
		CFileDirectTextReader(const IFixedString& strFileName);
		virtual ~CFileDirectTextReader();

		// IFile
		virtual bool				Validate(void) const;
		// ~IFile

		s8*							GetString(s8* pDstBuffer, size_t nDstBufferSize);
		IFixedString&				GetString(IFixedString& strString);

	protected:

		// Protected. Only CFileManager will be allowed to access Open/Close/Update.
		friend CFileManager;
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

#endif //_FILEDIRECTTEXTREADER_H_