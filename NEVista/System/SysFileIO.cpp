//----------------------------------------------------------//
// SYSFILEIO.CPP
//----------------------------------------------------------//
//-- Description
// Wrapper for file IO
//----------------------------------------------------------//


#include "Types.h"
#include "SysFileIO.h"


//----------------------------------------------------------//
// DEFINES
//----------------------------------------------------------//


//-- Define this to use safer fileIO functions in VC8 and VC9
#define SYSFILEIO_USES_SAFE_FILEIO


//----------------------------------------------------------//
// GLOBALS
//----------------------------------------------------------//

//----------------------------------------------------------//
// FileIO::Fopen
//----------------------------------------------------------//
//-- Description
// Wrapper around fopen with optional safe support
//----------------------------------------------------------//
FileIO::Handle FileIO::Fopen(const s8* strFilename, const s8* strMode)
{
#if defined(SYSFILEIO_USES_SAFE_FILEIO)
  
	Handle pFile;
	errno_t nError = fopen_s(&pFile, strFilename, strMode);
	if (nError == 0)
	{
		return pFile;
	}
	else
	{
		return NULL;
	}

#else
 
	return fopen(strFilename, strMode);

#endif
}


//----------------------------------------------------------//
// FileIO::Fclose
//----------------------------------------------------------//
//-- Description
// Wrapper around fclose
//----------------------------------------------------------//
s32 FileIO::Fclose(FileIO::Handle pFile)
{
	return fclose(pFile);
}


//----------------------------------------------------------//
// FileIO::Fflush
//----------------------------------------------------------//
//-- Description
// Wrapper around fflush
//----------------------------------------------------------//
s32 FileIO::Fflush(FileIO::Handle pFile)
{
	return fflush(pFile);
}


//----------------------------------------------------------//
// FileIO::Fseek
//----------------------------------------------------------//
//-- Description
// Wrapper around fseek
//----------------------------------------------------------//
s32 FileIO::Fseek(FileIO::Handle pFile, size_t nOffset, s32 nOrigin)
{
	return fseek(pFile, nOffset, nOrigin);
}


//----------------------------------------------------------//
// FileIO::Ftell
//----------------------------------------------------------//
//-- Description
// Wrapper around ftell
//----------------------------------------------------------//
s32 FileIO::Ftell(FileIO::Handle pFile)
{
	return ftell(pFile);
}


//----------------------------------------------------------//
// FileIO::Feof
//----------------------------------------------------------//
//-- Description
// Wrapper around feof
//----------------------------------------------------------//
s32 FileIO::Feof(FileIO::Handle pFile)
{
	return feof(pFile);
}


//----------------------------------------------------------//
// FileIO::Fprintf
//----------------------------------------------------------//
//-- Description
// Wrapper around fprintf with optional safe support
//----------------------------------------------------------//
s32 FileIO::Fprintf(FileIO::Handle pFile, const s8* strFormatting, ...)
{
	s32 nSymbolsConverted = -1;
	 
	if (!pFile || !strFormatting)
	{
		return nSymbolsConverted;
	}

	va_list ArgList;
	va_start(ArgList, strFormatting);

#if defined(SYSFILEIO_USES_SAFE_FILEIO)

	nSymbolsConverted = vfprintf_s(pFile, strFormatting, ArgList);

#else

	nSymbolsConverted = vfprintf(pFile, strFormatting, ArgList);

#endif

	va_end(ArgList);

	return nSymbolsConverted;
}


//----------------------------------------------------------//
// FileIO::Fputs
//----------------------------------------------------------//
//-- Description
// Wrapper around fputs
//----------------------------------------------------------//
s32 FileIO::Fputs(FileIO::Handle pFile, const s8* strBuffer)
{
	return fputs(strBuffer, pFile);
}


//----------------------------------------------------------//
// FileIO::Fgets
//----------------------------------------------------------//
//-- Description
// Wrapper around fgets
//----------------------------------------------------------//
s8* FileIO::Fgets(FileIO::Handle pFile, s8* strBuffer, size_t nCount)
{
	return fgets(strBuffer, nCount, pFile);
}
		
		
//----------------------------------------------------------//
// FileIO::Fprintf
//----------------------------------------------------------//
//-- Description
// Wrapper around fprintf with optional safe support
//----------------------------------------------------------//
size_t FileIO::Fread(FileIO::Handle pFile, size_t nSize, size_t nCount, void* pBuffer, size_t nBufferSize)
{
	if ((nSize * nCount) > nBufferSize)
	{
		nCount = nBufferSize / nSize;
	}

	return fread(pBuffer, nSize, nCount, pFile);
}


//----------------------------------------------------------//
// FileIO::Fprintf
//----------------------------------------------------------//
//-- Description
// Wrapper around fprintf with optional safe support
//----------------------------------------------------------//
size_t FileIO::Fwrite(FileIO::Handle pFile, size_t nSize, size_t nCount, const void* pBuffer, size_t nBufferSize)
{
	if ((nSize * nCount) > nBufferSize)
	{
		nCount = nBufferSize / nSize;
	}

	return fwrite(pBuffer, nSize, nCount, pFile);
}


//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//
