#ifndef _FILEDATA_H_
#define _FILEDATA_H_
#pragma once

//----------------------------------------------------------//
// FILEDATA.H
//----------------------------------------------------------//
//-- Description
// CFileData is a structure to be held in lists within the 
// FileManager containing the actual file status. 
// This data should not be accessible directly:
// Use a FileAccessor to read/write to the file, and let the 
// FileManager do all the updating via a FileProcessor.
//----------------------------------------------------------//


#include "Types.h"
#include "SysString.h"
#include "SysFileIO.h"


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


class CFileAccessor;
class CFileAccessorDirectReader;
class CFileAccessorDirectTextReader;
class CFileAccessorDirectBinaryReader;
class CFileAccessorDirectWriter;
class CFileAccessorDirectTextWriter;
class CFileAccessorDirectBinaryWriter;

class CFileProcessor;
class CFileProcessorDirectReader;
class CFileProcessorDirectTextReader;
class CFileProcessorDirectBinaryReader;
class CFileProcessorDirectWriter;
class CFileProcessorDirectTextWriter;
class CFileProcessorDirectBinaryWriter;


//----------------------------------------------------------//
// CLASSES
//----------------------------------------------------------//


class CFileData
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
				Any				= 0xff
			};
		};

	private:

		struct SAttachedBuffer
		{
			u8*								m_pBuffer;					
			size_t							m_nBufferSize;
			size_t							m_nFilled;
			size_t							m_nOffset;
			u32								m_nUnlockRefCount;
		};

		struct SFileDirectReaderData
		{
			SysFileIO::Handle					m_pFile;
			size_t							m_nSize;
		};
		struct SFileDirectWriterData
		{
			SysFileIO::Handle					m_pFile;
			size_t							m_nWritten;
		};

		struct SFileBufferedReaderData
		{
			SysFileIO::Handle					m_pFile;
			size_t							m_nSize;
			size_t							m_nRead;
			SAttachedBuffer					m_Buffer;
		};
		struct SFileBufferedWriterData
		{
			SysFileIO::Handle					m_pFile;
			size_t							m_nSize;
			size_t							m_nWritten;
			SAttachedBuffer					m_Buffer;
		};

//		struct SArchiveEntryData
//		{
//			SFileInfo*						m_pParentArchiveInfo;
//			size_t							m_nSize;
//			size_t							m_nRead;
//			SAttachedBuffer					m_Buffer;
//		};

//		struct SArchiveFileReaderData
//		{
//			SysFileIO::Handle					m_pFile;
//			size_t							m_nSize;
//			size_t							m_nRead;
//			size_t							m_nHeaderTOCSize;
//			u8*								m_pHeaderTOC;
//			SAttachedBuffer					m_Buffer;
//		};

		union
		{
			SFileDirectReaderData			m_DirectReaderData;
			SFileDirectWriterData			m_DirectWriterData;

			SFileBufferedReaderData			m_BufferedReaderData;
			SFileBufferedWriterData			m_BufferedWriterData;
//			SArchiveEntryData				m_BufferedArcEntryReaderData;

//			SFileBufferedReaderData			m_AsyncTextReaderData;
//			SFileBufferedWriterData			m_AsyncTextWriterData;
//			SFileBufferedReaderData			m_AsyncBinaryReaderData;
//			SFileBufferedWriterData			m_AsyncBinaryWriterData;
//			SArchiveEntryData				m_AsyncArcEntryReaderData;

//			SFileBufferedReaderData			m_StreamedBinaryReaderData;

//			SArchiveFileReaderData			m_ArchiveBufferReaderData;
//			SArchiveFileReaderData			m_ArchiveAsyncReaderData;
		};

		FixedString<FILE_MAX_FILENAME_LENGTH> m_strFileName;
		SysString::Hash						m_nHash;
		Type::Enum							m_eFileType;
		AccessMethod::Enum					m_eAccessMethod;

	public:

		CFileData(const s8* strFileName, Type::Enum eType, AccessMethod::Enum eAccess);
		~CFileData();

		bool								Validate(Type::Enum eType, AccessMethod::Enum eAccess) const;

		Type::Enum							GetFileType(void) const;
		AccessMethod::Enum					GetAccessMethod(void) const;
		const s8*							GetFileName(void) const;
		SysString::Hash						GetHash(void) const;

	friend CFileAccessor;
	friend CFileAccessorDirectReader;
	friend CFileAccessorDirectTextReader;
//	friend CFileAccessorDirectBinaryReader;
	friend CFileAccessorDirectWriter;
	friend CFileAccessorDirectTextWriter;
//	friend CFileAccessorDirectBinaryWriter;

	friend CFileProcessor;
	friend CFileProcessorDirectReader;
	friend CFileProcessorDirectTextReader;
//	friend CFileProcessorDirectBinaryReader;
	friend CFileProcessorDirectWriter;
	friend CFileProcessorDirectTextWriter;
//	friend CFileProcessorDirectBinaryWriter;
};


//----------------------------------------------------------//
// EXTERNALS
//----------------------------------------------------------//

//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//

#endif //_FILEDATA_H_