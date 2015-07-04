//----------------------------------------------------------//
// FILEMANAGER.CPP
//----------------------------------------------------------//
//-- Description
// CFileManager contains a lists of open direct and buffered
// files that can be queried, processed asynchronously, etc.
//----------------------------------------------------------//


#include "FileManager.h"
#include "FileData.h"
#include "Types.h"
#include "SysString.h"


//----------------------------------------------------------//
// DEFINES
//----------------------------------------------------------//

//----------------------------------------------------------//
// GLOBALS
//----------------------------------------------------------//


CFileManager* CFileManager::m_pSingleton = NULL;


//----------------------------------------------------------//
// CFileManager::CFileManager
//----------------------------------------------------------//
//-- Description			
//----------------------------------------------------------//
CFileManager::CFileManager()
{
	m_DirectFiles.clear();
	m_BufferedFiles.clear();
}


//----------------------------------------------------------//
// CFileManager::~CFileManager
//----------------------------------------------------------//
//-- Description			
//----------------------------------------------------------//
CFileManager::~CFileManager()
{
	Reset();
}


//----------------------------------------------------------//
// CFileManager::Instance
//----------------------------------------------------------//
//-- Description			
//----------------------------------------------------------//
const CFileManager* CFileManager::Instance(void)
{
	if (IS_NULL_PTR(m_pSingleton))
	{
		m_pSingleton = new CFileManager();
	}
	return m_pSingleton;
}


//----------------------------------------------------------//
// CFileManager::Shutdown
//----------------------------------------------------------//
//-- Description			
//----------------------------------------------------------//
CFileManager::Error::Enum CFileManager::Reset(void)
{
	//-- For each open file, clean up
	//-- Direct files can be cleaned up instantly.
	m_DirectFiles.clear();

	//-- Buffered files might need more processing
	m_BufferedFiles.clear();

	return Error::Ok;
}


//----------------------------------------------------------//
// CFileManager::Open
//----------------------------------------------------------//
//-- Description
// Returns a pointer to open file if it exists, even if
// the file already existed.
// If returned pointer is not null, you should always test
// the type of the file to confirm it is the same as the 
// one requested to open.
//----------------------------------------------------------//
const CFileData* CFileManager::Open(s8* strFileName, CFileData::Type::Enum eType, CFileData::AccessMethod::Enum eAccess)
{
	switch (eAccess)
	{
		case CFileData::AccessMethod::DirectRead:
		case CFileData::AccessMethod::DirectWrite:
		{
			const CFileData* pFile = Find(strFileName);
			if (IS_PTR(pFile))
			{
				//-- Already exists.
				return pFile;
			}
			else
			{
				//-- Not found. Add it.
				m_DirectFiles.push_back(CFileData(strFileName, eType, eAccess));
				return &(m_DirectFiles.back());
			}
		}
		break;

		case CFileData::AccessMethod::BufferedRead:
		case CFileData::AccessMethod::BufferedWrite:
		case CFileData::AccessMethod::AsyncRead:
		case CFileData::AccessMethod::AsyncWrite:
		case CFileData::AccessMethod::StreamedRead:
		{
			const CFileData* pFile = Find(strFileName);
			if (IS_PTR(pFile))
			{
				//-- Already exists.
				return pFile;
			}
			else
			{
				//-- Not found. Add it.
				m_BufferedFiles.push_back(CFileData(strFileName, eType, eAccess));
				return &(m_BufferedFiles.back());
			}
		}
		break;

		default:
		{
			return NULL;
		}
		break;
	}

	return NULL;
}


//----------------------------------------------------------//
// CFileManager::Open
//----------------------------------------------------------//
//-- Description			
// Close an open file.
//----------------------------------------------------------//
CFileManager::Error::Enum CFileManager::Close(SysString::Hash nHash)
{
	if (SysString::INVALID_HASH != nHash)
	{
		for (TFileList::const_iterator cit = m_DirectFiles.begin(); cit != m_DirectFiles.end(); ++cit)
		{
			if (cit->GetHash() == nHash)
			{
				m_DirectFiles.erase(cit);
				return Error::Ok;
			}
		}

		for (TFileList::const_iterator cit = m_BufferedFiles.begin(); cit != m_BufferedFiles.end(); ++cit)
		{
			if (cit->GetHash() == nHash)
			{
				m_BufferedFiles.erase(cit);
				return Error::Ok;
			}
		}
	}

	return Error::FileNotFound;
}

CFileManager::Error::Enum CFileManager::Close(const CFileData* pFile)
{
	if (IS_PTR(pFile))
	{
		return Close(pFile->GetHash());
	}

	return Error::FileNotFound;
}

CFileManager::Error::Enum CFileManager::Close(const s8* strFileName)
{
	return Close(SysString::GenerateHash(strFileName));
}


//----------------------------------------------------------//
// CFileManager::Exists
//----------------------------------------------------------//
//-- Description			
// Does a file exist in the manager?
//----------------------------------------------------------//
bool CFileManager::Exists(const CFileData* pFile) const
{
	for (TFileList::const_iterator cit = m_DirectFiles.begin(); cit != m_DirectFiles.end(); ++cit)
	{
		if (cit->GetHash() == pFile->GetHash())
		{
			return true;
		}
	}

	for (TFileList::const_iterator cit = m_BufferedFiles.begin(); cit != m_BufferedFiles.end(); ++cit)
	{
		if (cit->GetHash() == pFile->GetHash())
		{
			return true;
		}
	}

	return false;
}


//----------------------------------------------------------//
// CFileManager::Find
//----------------------------------------------------------//
//-- Description			
// Find and return a file from the manager.
//----------------------------------------------------------//
const CFileData* CFileManager::Find(SysString::Hash nHash) const
{
	if (SysString::INVALID_HASH != nHash)
	{
		for (TFileList::const_iterator cit = m_DirectFiles.begin(); cit != m_DirectFiles.end(); ++cit)
		{
			if (cit->GetHash() == nHash)
			{
				return &(*cit);
			}
		}

		for (TFileList::const_iterator cit = m_BufferedFiles.begin(); cit != m_BufferedFiles.end(); ++cit)
		{
			if (cit->GetHash() == nHash)
			{
				return &(*cit);
			}
		}
	}

	return NULL;
}

const CFileData* CFileManager::Find(const s8* strFileName) const
{
	return Find(SysString::GenerateHash(strFileName));
}
