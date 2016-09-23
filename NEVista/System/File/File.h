#ifndef _FILE_H_
#define _FILE_H_
#pragma once

//----------------------------------------------------------//
// FILE.H
//----------------------------------------------------------//
//-- Description
// CFile base class.
//----------------------------------------------------------//


#include "IFile.h"
#include "Types.h"
#include "SysString.h"
#include "FixedString.h"


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


class IFixedString;


//----------------------------------------------------------//
// CLASSES
//----------------------------------------------------------//


class CFile : public IFile
{
	public:

		struct Error
		{
			enum Enum
			{
				FileAlreadyOpen					= 0x80000003,
				FileOpenFailed					= 0x80000002,
				FileNotFound					= 0x80000001,
				Failed							= -1,
				Ok								= 0
			};
		};

		CFile(const s8* strFileName, Type::Enum eType, AccessMethod::Enum eAccess);
		CFile(const IFixedString& strFileName, Type::Enum eType, AccessMethod::Enum eAccess);
		virtual ~CFile();

		// IFile
		virtual Type::Enum						GetFileType(void) const;
		virtual AccessMethod::Enum				GetAccessMethod(void) const;
		virtual const IFixedString&				GetFileName(void) const;
		virtual SysString::Hash					GetHash(void) const;

		virtual bool							IsTypeAccess(Type::Enum eType, AccessMethod::Enum eAccess) const;
		// ~IFile

	protected:

		// Protected. Only CFileManager will be allowed to access Open/Close/Update, via derived classes.
//		virtual Error::Enum						Open(void) = 0;
//		virtual Error::Enum						Close(void) = 0;
//		virtual Error::Enum						Update(void) = 0;

		FixedString<FILE_MAX_FILENAME_LENGTH>	m_strFileName;
		SysString::Hash							m_nHash;
		Type::Enum								m_eFileType;
		AccessMethod::Enum						m_eAccessMethod;
};


//----------------------------------------------------------//
// EXTERNALS
//----------------------------------------------------------//

//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//

#endif //_FILE_H_