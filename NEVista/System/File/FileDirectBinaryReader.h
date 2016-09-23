#ifndef _FILEDIRECTBINARYREADER_H_
#define _FILEDIRECTBINARYREADER_H_
#pragma once

//----------------------------------------------------------//
// FILEDIRECTBINARYREADER.H
//----------------------------------------------------------//
//-- Description
// CFileDirectBinaryReader class. Derived from
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

//----------------------------------------------------------//
// CLASSES
//----------------------------------------------------------//


class CFileDirectBinaryReader : public CFileDirectReader
{
	public:

		CFileDirectBinaryReader(const s8* strFileName);
		CFileDirectBinaryReader(const IFixedString& strFileName);
		virtual ~CFileDirectBinaryReader();

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

#endif //_FILEDIRECTBINARYREADER_H_