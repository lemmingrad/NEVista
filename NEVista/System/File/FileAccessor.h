#ifndef _FILEACCESSOR_H_
#define _FILEACCESSOR_H_
#pragma once

//----------------------------------------------------------//
// FILEACCESSOR.H
//----------------------------------------------------------//
//-- Description
// CFileAccessor utility class.
//----------------------------------------------------------//


#include "FileData.h"


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


class CFileAccessor
{
	protected:

		CFileData*						m_pData;

	public:

		CFileAccessor(CFileData* pData);
		virtual ~CFileAccessor();
	
		const s8*						GetFileName(void) const;
		String::Hash					GetHash(void) const;
		CFileData::Type::Enum			GetFileType(void) const;
		CFileData::AccessMethod::Enum	GetAccessMethod(void) const;

		virtual bool					ValidateData(void) const = 0;
		virtual bool					IsOpen(void) const = 0;

//		size_t							Read(size_t nRequestedSize, void* pDstBuffer, size_t nDstBufferSize);
//		s8*								GetString(s8* pDstBuffer, size_t nDstBufferSize);
//		size_t							Write(size_t nRequestedSize, const void* pSrcBuffer, size_t nSrcBufferSize);
//		size_t							Printf(const s8* strFormatting, ...);
//		size_t							PutString(const s8* strSrcBuffer);
};


//----------------------------------------------------------//
// EXTERNALS
//----------------------------------------------------------//

//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//

#endif //_FILEACCESSOR_H_