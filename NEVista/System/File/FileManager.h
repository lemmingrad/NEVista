#ifndef _FILEMANAGER_H_
#define _FILEMANAGER_H_
#pragma once

//----------------------------------------------------------//
// FILEMANAGER.H
//----------------------------------------------------------//
//-- Description
// CFileManager contains a lists of open direct and buffered
// files that can be queried, processed asynchronously, etc.
//----------------------------------------------------------//


#include <list>


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


class CFileManager
{
	public:

		struct Error
		{
			enum Enum
			{
				NotOpen = 0x80000001,

				FileNotFound = 0x80000002,
				
				
				Failed = -1,
				Ok = 0,
			};
		};

		CFileManager();
		~CFileManager();

		static const CFileManager*		Instance(void);
		Error::Enum						Reset(void);

		const CFileData*				Open(s8* strFileName, CFileData::Type::Enum eType, CFileData::AccessMethod::Enum eAccess);

		Error::Enum						Close(const CFileData* pFile);
		Error::Enum						Close(String::Hash nHash);
		Error::Enum						Close(const s8* strFileName);

		bool							Exists(const CFileData* pFile) const;
		const CFileData*				Find(String::Hash nHash) const;
		const CFileData*				Find(const s8* strFileName) const;

	private:

		static CFileManager*			m_pSingleton;

		typedef std::list<CFileData>	TFileList;
		TFileList						m_DirectFiles;
		TFileList						m_BufferedFiles;
};


//----------------------------------------------------------//
// EXTERNALS
//----------------------------------------------------------//

//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//

#endif //_FILEMANAGER_H_