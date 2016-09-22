#ifndef _IFILE_H_
#define _IFILE_H_
#pragma once

//----------------------------------------------------------//
// IFILE.H
//----------------------------------------------------------//
//-- Description
// Interface for a CFile Object
//----------------------------------------------------------//


#include "Types.h"
#include "SysString.h"


//----------------------------------------------------------//
// DEFINES
//----------------------------------------------------------//


#define FILE_MAX_FILENAME_LENGTH					(256)


//----------------------------------------------------------//
// ENUMS
//----------------------------------------------------------//

//----------------------------------------------------------//
// STRUCTS
//----------------------------------------------------------//

//----------------------------------------------------------//
// FORWARD REFERENCES
//----------------------------------------------------------//


class IFixedString;


//----------------------------------------------------------//
// CLASSES
//----------------------------------------------------------//


class IFile
{
	public:

		struct Type
		{
			enum Enum
			{
				Unknown = 0,
				Text,
				Binary,
				ArcEntry,
				Archive,
				Any
			};
		};

		struct AccessMethod
		{
			enum Enum
			{
				Unknown			= 0x00,
				DirectRead		= 0x01,
				DirectWrite		= 0x11,
				BufferedRead	= 0x02,
				BufferedWrite	= 0x12,
				AsyncRead		= 0x04,
				AsyncWrite		= 0x14,
				StreamedRead	= 0x08,
				Any				= 0xFF
			};
		};

		virtual ~IFile() {}

		virtual Type::Enum						GetFileType(void) const = 0;
		virtual AccessMethod::Enum				GetAccessMethod(void) const = 0;
		virtual const IFixedString&				GetFileName(void) const = 0;
		virtual SysString::Hash					GetHash(void) const = 0;

		virtual bool							Validate(void) = 0;
		virtual bool							IsOpen(void) const = 0;
		virtual bool							IsTypeAccess(Type::Enum eType, AccessMethod::Enum eAccess) const = 0;
};


//----------------------------------------------------------//
// EXTERNALS
//----------------------------------------------------------//

//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//

#endif //_IFILE_H_