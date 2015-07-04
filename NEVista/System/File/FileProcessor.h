#ifndef _FILEPROCESSOR_H_
#define _FILEPROCESSOR_H_
#pragma once

//----------------------------------------------------------//
// FILEPROCESSOR.H
//----------------------------------------------------------//
//-- Description
// CFileProcessor utility class.
//----------------------------------------------------------//


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


class CFileProcessor
{
	protected:

		CFileData*								m_pData;

	public:

		struct Error
		{
			enum Enum
			{
				FileNotFound					= 0x80000001,
				Failed							= -1,
				Ok								= 0
			};
		};

		CFileProcessor(CFileData* pData);
		virtual ~CFileProcessor();

		virtual bool							ValidateData(void) const = 0;
		virtual bool							IsOpen(void) const = 0;

		virtual Error::Enum						Open(void) = 0;
		virtual Error::Enum						Close(void) = 0;
		virtual Error::Enum						Update(void) = 0;
};


//----------------------------------------------------------//
// EXTERNALS
//----------------------------------------------------------//

//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//

#endif //_FILEPROCESSOR_H_