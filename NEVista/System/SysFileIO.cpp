//----------------------------------------------------------//
// SYSFILEIO.CPP
//----------------------------------------------------------//
//-- Description
// Wrapper for file IO
//----------------------------------------------------------//


#include "SysFileIO.h"
#include "Types.h"
#include <cstdio>


//----------------------------------------------------------//
// DEFINES
//----------------------------------------------------------//


//-- Define this to use safer fileIO functions in VC8 and VC9
#define SYSFILEIO_USES_SAFE_FILEIO


//----------------------------------------------------------//
// GLOBALS
//----------------------------------------------------------//


const SysFileIO::Handle SysFileIO::INVALID_HANDLE = (SysFileIO::Handle)NULL;


//----------------------------------------------------------//
// SysFileIO::Fopen
//----------------------------------------------------------//
//-- Description
// Wrapper around fopen with optional safe support
//----------------------------------------------------------//
SysFileIO::Handle SysFileIO::Fopen(const s8* strFilename, const s8* strMode)
{
#if defined(SYSFILEIO_USES_SAFE_FILEIO)
  
	Handle pFile;
	errno_t nError = fopen_s(&pFile, strFilename, strMode);
	if (IS_ZERO(nError))
	{
		return pFile;
	}
	else
	{
		return SysFileIO::INVALID_HANDLE;
	}

#else
 
	return fopen(strFilename, strMode);

#endif
}


//----------------------------------------------------------//
// SysFileIO::Fclose
//----------------------------------------------------------//
//-- Description
// Wrapper around fclose
//----------------------------------------------------------//
s32 SysFileIO::Fclose(SysFileIO::Handle pFile)
{
	return fclose(pFile);
}


//----------------------------------------------------------//
// SysFileIO::Fflush
//----------------------------------------------------------//
//-- Description
// Wrapper around fflush
//----------------------------------------------------------//
s32 SysFileIO::Fflush(SysFileIO::Handle pFile)
{
	return fflush(pFile);
}


//----------------------------------------------------------//
// SysFileIO::Fseek
//----------------------------------------------------------//
//-- Description
// Wrapper around fseek
//----------------------------------------------------------//
s32 SysFileIO::Fseek(SysFileIO::Handle pFile, size_t nOffset, s32 nOrigin)
{
	return fseek(pFile, nOffset, nOrigin);
}


//----------------------------------------------------------//
// SysFileIO::Ftell
//----------------------------------------------------------//
//-- Description
// Wrapper around ftell
//----------------------------------------------------------//
s32 SysFileIO::Ftell(SysFileIO::Handle pFile)
{
	return ftell(pFile);
}


//----------------------------------------------------------//
// SysFileIO::Feof
//----------------------------------------------------------//
//-- Description
// Wrapper around feof
//----------------------------------------------------------//
s32 SysFileIO::Feof(SysFileIO::Handle pFile)
{
	return feof(pFile);
}


//----------------------------------------------------------//
// SysFileIO::Fprintf
//----------------------------------------------------------//
//-- Description
// Wrapper around fprintf with optional safe support
//----------------------------------------------------------//
s32 SysFileIO::Fprintf(SysFileIO::Handle pFile, const s8* strFormatting, ...)
{
	s32 nSymbolsConverted = -1;
	 
	if ( (SysFileIO::INVALID_HANDLE == pFile)
		|| IS_NULL_PTR(strFormatting) )
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
// SysFileIO::Fputs
//----------------------------------------------------------//
//-- Description
// Wrapper around fputs
//----------------------------------------------------------//
s32 SysFileIO::Fputs(SysFileIO::Handle pFile, const s8* strBuffer)
{
	return fputs(strBuffer, pFile);
}


//----------------------------------------------------------//
// SysFileIO::Fgets
//----------------------------------------------------------//
//-- Description
// Wrapper around fgets
//----------------------------------------------------------//
s8* SysFileIO::Fgets(SysFileIO::Handle pFile, s8* strBuffer, size_t nCount)
{
	return fgets(strBuffer, nCount, pFile);
}
		
		
//----------------------------------------------------------//
// SysFileIO::Fprintf
//----------------------------------------------------------//
//-- Description
// Wrapper around fprintf with optional safe support
//----------------------------------------------------------//
size_t SysFileIO::Fread(SysFileIO::Handle pFile, size_t nSize, size_t nCount, void* pBuffer, size_t nBufferSize)
{
	if ((nSize * nCount) > nBufferSize)
	{
		nCount = nBufferSize / nSize;
	}

	return fread(pBuffer, nSize, nCount, pFile);
}


//----------------------------------------------------------//
// SysFileIO::Fprintf
//----------------------------------------------------------//
//-- Description
// Wrapper around fprintf with optional safe support
//----------------------------------------------------------//
size_t SysFileIO::Fwrite(SysFileIO::Handle pFile, size_t nSize, size_t nCount, const void* pBuffer, size_t nBufferSize)
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
