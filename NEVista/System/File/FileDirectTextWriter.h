#ifndef _FILEDIRECTTEXTWRITER_H_
#define _FILEDIRECTTEXTWRITER_H_
#pragma once

//----------------------------------------------------------//
// FILEDIRECTTEXTWRITER.H
//----------------------------------------------------------//
//-- Description
// CFileDirectTextWriter class. Derived from
// CFileDirectWriter.
//----------------------------------------------------------//


#include "FileDirectWriter.h"
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

//----------------------------------------------------------//
// CLASSES
//----------------------------------------------------------//


class CFileDirectTextWriter : public CFileDirectWriter
{
	public:

		CFileDirectTextWriter(const s8* strFileName);
		CFileDirectTextWriter(const IFixedString& strFileName);
		virtual ~CFileDirectTextWriter();

		// IFile
		virtual bool				Validate(void) const;
		// ~IFile

		size_t						Printf(const s8* strFormatting, ...);
		size_t						PutString(const s8* pSrcBuffer);
		size_t						PutString(const std::string& strString);
		size_t						PutString(const IFixedString& strString);

		//-- In DirectReader and DirectWriter, Open and Close are public
		//-- so they can be used anywhere.
		virtual Error::Enum			Open(void);
		virtual Error::Enum			Close(void);
			
	private:
		
		//-- In DirectReader and DirectWriter. Update is private and never used.
		virtual Error::Enum			Update(void);
};


//----------------------------------------------------------//
// EXTERNALS
//----------------------------------------------------------//

//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//

#endif //_FILEDIRECTTEXTWRITER_H_