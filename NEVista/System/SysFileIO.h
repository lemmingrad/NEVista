#ifndef _SYS_FILEIO_H_
#define _SYS_FILEIO_H_
#pragma once

//----------------------------------------------------------//
// SYSFILEIO.H
//----------------------------------------------------------//
//-- Description
// Wrapper for file IO
//----------------------------------------------------------//


#include "Types.h"
#include <cstdio>


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


class SysFileIO
{
	public:

		typedef FILE*				Handle;
		static const Handle			INVALID_HANDLE;

		static Handle				Fopen(const s8* strFilename, const s8* strMode);
		static s32					Fclose(Handle pFile); 
		static s32					Fflush(Handle pFile);

		static s32					Fseek(Handle pFile, size_t nOffset, s32 nOrigin);
		static s32					Ftell(Handle pFile);
		static s32					Feof(Handle pFile);

		static s32					Fprintf(Handle pFile, const s8* strFormatting, ...);
		static s32					Fputs(Handle pFile, const s8* strBuffer);
		static s8*					Fgets(Handle pFile, s8* strBuffer, size_t nCount);

		static size_t				Fread(Handle pFile, size_t nSize, size_t nCount, void* pBuffer, size_t nBufferSize);
		static size_t				Fwrite(Handle pFile, size_t nSize, size_t nCount, const void* pBuffer, size_t nBufferSize);

	private:
			
		SysFileIO();
		~SysFileIO();
};


//----------------------------------------------------------//
// EXTERNALS
//----------------------------------------------------------//

//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//

#endif
