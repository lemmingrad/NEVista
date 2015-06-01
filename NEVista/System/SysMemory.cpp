//----------------------------------------------------------//
// SYSMEMORY.CPP
//----------------------------------------------------------//
//-- Description
// Wrapper for memory access, memory logging and a very basic
// memory manager
//----------------------------------------------------------//


#include <windows.h>
#include <malloc.h>

#include "Types.h"

#include "SysMemory.h"


//----------------------------------------------------------//
// DEFINES
//----------------------------------------------------------//


//-- enable for big endian mode
//#define SYSMEMORY_BIG_ENDIAN

//-- use safe memory functions, as seen in VC8 and beyond.
//-- NOTE this will override the optimised functions if used below.
#define SYSMEMORY_USES_SAFE_MEMORY

//-- define this to use Windows optimised functions like ZeroMemory instead of memset, etc.
#define SYSMEMORY_USES_WINDOWS_MEMORY_OPTIMISATION


//----------------------------------------------------------//
// GLOBALS
//----------------------------------------------------------//

//----------------------------------------------------------//
// Memory::Memcpy
//----------------------------------------------------------//
//-- Description
// Copies a number of bytes from one address to another 
// address
//----------------------------------------------------------//
void* Memory::Memcpy(void* pDest, size_t nDestSize, const void* pSrc, size_t nSrcSize)
{
#if defined(SYSMEMORY_USES_SAFE_MEMORY)

    errno_t nError = memcpy_s(pDest, nDestSize, pSrc, nSrcSize);
    if (nError == 0)
    {
        return pDest;
    }
    else
    {
        return NULL;
    }

#else
	
    if (!pDest || (nDestSize == 0) || !pSrc || (nSrcSize == 0))
    {
        //-- Be consistent with the behavior of memcpy_s.
        return NULL;
    }
	
    if (nSrcSize > nDestSize)
    {
        //-- The source memory is too large to copy to the destination.  To
        //-- be consistent with memcpy_s, return null as an indication that the
        //-- copy failed.
        return NULL;
    }

#	if defined(SYSMEMORY_USES_WINDOWS_MEMORY_OPTIMISATION)

		CopyMemory(pDest, pSrc, nSrcSize);
		return pDest;

#	else

		return memcpy(pDest, pSrc, nSrcSize);

#	endif

#endif
}


//----------------------------------------------------------//
// Memory::Memmove
//----------------------------------------------------------//
//-- Description
// Move a number of bytes from one address to another 
// address. Should safely handle cases where src and dest
// overlap in memory.
//----------------------------------------------------------//
void* Memory::Memmove(void* pDest, size_t nDestSize, const void* pSrc, size_t nSrcSize)
{
#if defined(SYSMEMORY_USES_SAFE_MEMORY)

    errno_t nError = memmove_s(pDest, nDestSize, pSrc, nSrcSize);
    if (nError == 0)
    {
        return pDest;
    }
    else
    {
        return NULL;
    }

#else
	
    if (!pDest || (nDestSize == 0) || !pSrc || (nSrcSize == 0))
    {
        //-- Be consistent with the behavior of memmove_s.
        return NULL;
    }
	
    if (nSrcSize > nDestSize)
    {
        //-- The source memory is too large to copy to the destination.  To
        //-- be consistent with memcpy_s, return null as an indication that the
        //-- copy failed.
        return NULL;
    }

#	if defined(SYSMEMORY_USES_WINDOWS_MEMORY_OPTIMISATION)

		MoveMemory(pDest, pSrc, nSrcSize);
		return pDest;

#	else

		return memmove(pDest, pSrc, nSrcSize);

#	endif

#endif
}


//----------------------------------------------------------//
// Memory::Memset
//----------------------------------------------------------//
//-- Description
// Sets each byte in a block of memory to a specific value
//----------------------------------------------------------//
void* Memory::Memset(void* pMem, s8 nValue, size_t nByteCount)
{
    if (!pMem || (nByteCount == 0))
    {
        return NULL;
    }

#	if defined(SYSMEMORY_USES_WINDOWS_MEMORY_OPTIMISATION)

		FillMemory(pMem, nByteCount, nValue);
		return pMem;

#	else

		return memset(pMem, nValue, nByteCount);

#	endif
}


//----------------------------------------------------------//
// Memory::Memclear
//----------------------------------------------------------//
//-- Description
// Sets a block of memory to 0
//----------------------------------------------------------//
void* Memory::Memclear(void* pMem, size_t nByteCount)
{
    if (!pMem || (nByteCount == 0))
    {
        return NULL;
    }

#	if defined(SYSMEMORY_USES_WINDOWS_MEMORY_OPTIMISATION)

		ZeroMemory(pMem, nByteCount);
		return pMem;

#	else

		return memset(pMem, 0, nByteCount);

#	endif
}


//----------------------------------------------------------//
// Memory::EndianSwapValue
//----------------------------------------------------------//
//-- Description
// Reverses the order of a block of bytes.
// Like switching the bytes in a s32, etc.
//----------------------------------------------------------//
void Memory::EndianSwap(void* pMem, size_t nSizeofEntity)
{
	//-- size must be even
	assert((nSizeofEntity >= 2) && ((nSizeofEntity & 1) == 0));
	//-- limit to 8 bytes max.
	assert(nSizeofEntity <= sizeof(u64));

	u8* acBytes = (u8*)pMem;
	for (u32 i = 0, j = (nSizeofEntity - 1); i < (nSizeofEntity / 2); ++i, --j)
	{
		u8 cTemp = acBytes[i];
		acBytes[i] = acBytes[j];
		acBytes[j] = cTemp;
	}
}


//----------------------------------------------------------//
// Memory::EndianSwapArray
//----------------------------------------------------------//
//-- Description
// Reverses the bytes for a number of entries in an array
// for example, endian swapping an array of s32, etc.
//----------------------------------------------------------//
void Memory::EndianSwapArray(void *pMem, size_t nSizeofEntity, u32 nEntityCount)
{
	//-- size must be even
	assert((nSizeofEntity >= 2) && ((nSizeofEntity & 1) == 0));
	//-- limit to 8 bytes max.
	assert(nSizeofEntity <= sizeof(u64));

	u8* acBytes = (u8*)pMem;
	for (u32 i = 0; i < nEntityCount; ++i, acBytes += nSizeofEntity)
	{
		Memory::EndianSwap((void*)acBytes, nSizeofEntity);
	}
}


//----------------------------------------------------------//
// Memory::EndianCopyValue
//----------------------------------------------------------//
//-- Description
// Copies a value, reversing the endian.
//----------------------------------------------------------//
void Memory::EndianSwapCopy(void* pDest, const void* pSrc, size_t nSizeofEntity)
{
	//-- size must be even
	assert((nSizeofEntity >= 2) && ((nSizeofEntity & 1) == 0));
	//-- limit to 8 bytes max.
	assert(nSizeofEntity <= sizeof(u64));

	Memory::Memcpy(pDest, nSizeofEntity, pSrc, nSizeofEntity);
	Memory::EndianSwap(pDest, nSizeofEntity);
}


//----------------------------------------------------------//
// Memory::EndianCopyArray
//----------------------------------------------------------//
//-- Description
// Copies a block of values, reversing the endian of 
// each value.
//----------------------------------------------------------//
void Memory::EndianSwapArrayCopy(void* pDest, const void* pSrc, size_t nSizeofEntity, u32 nEntityCount)
{
	//-- size must be even
	assert((nSizeofEntity >= 2) && ((nSizeofEntity & 1) == 0));
	//-- limit to 8 bytes max.
	assert(nSizeofEntity <= sizeof(u64));

	size_t nTotalSize = nSizeofEntity * nEntityCount;

	Memory::Memcpy(pDest, nTotalSize, pSrc, nTotalSize);
	Memory::EndianSwapArray(pDest, nSizeofEntity, nEntityCount);
}


//----------------------------------------------------------//
// Memory::IsSystemBigEndian
//----------------------------------------------------------//
//-- Description
// Determines if a system is big or little endian
//----------------------------------------------------------//
bool Memory::IsSystemBigEndian(void)
{
    s32 nInt = 1;
    s8* pcTemp = (s8*)&nInt;
    return !(*pcTemp);
}


//----------------------------------------------------------//
// Memory::Align
//----------------------------------------------------------//
//-- Description
// Given a pointer, aligns the pointer to the nearest byte
// boundary specified and returns a new pointer
//----------------------------------------------------------//
void* Memory::Align(const void* pMem, size_t nAlignment)
{
	uintptr_t nAddress = (uintptr_t)pMem;
	--nAlignment;
	nAddress += nAlignment;
	nAddress &= ~nAlignment;

	return (void*)nAddress;
}


//----------------------------------------------------------//
// Memory::Pad
//----------------------------------------------------------//
//-- Description
// Given a size, pads the size to the nearest byte
// boundary specified and returns a new size
//----------------------------------------------------------//
size_t Memory::Pad(size_t nSize, size_t nAlignment)
{
	size_t nPadded = nSize;
	--nAlignment;
	nPadded += nAlignment;
	nPadded &= ~nAlignment;

	return nPadded;
}


//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//
