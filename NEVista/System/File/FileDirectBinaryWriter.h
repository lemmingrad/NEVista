#ifndef _FILEDIRECTBINARYWRITER_H_
#define _FILEDIRECTBINARYWRITER_H_
#pragma once

//----------------------------------------------------------//
// FILEDIRECTBINARYWRITER.H
//----------------------------------------------------------//
//-- Description
// CFileDirectBinaryWriter class. Derived from
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


class CFileDirectBinaryWriter : public CFileDirectWriter
{
	public:

		static const size_t PRINTF_BUFFER_SIZE = 1024;

		CFileDirectBinaryWriter(const s8* strFileName);
		CFileDirectBinaryWriter(const IFixedString& strFileName);
		virtual ~CFileDirectBinaryWriter();

		// IFile
		virtual bool				Validate(void) const;
		// ~IFile

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

#endif //_FILEDIRECTBINARYWRITER_H_