//----------------------------------------------------------//
// SYSMEMORY.CPP
//----------------------------------------------------------//
//-- Description
// Wrapper for memory access, memory logging and a very basic
// memory manager
//----------------------------------------------------------//


#include "SysMemory.h"
#include "Types.h"
#include <cstdio>
#include <cstdlib>
#include <memory.h>
#if defined(WIN32)
#	include <windows.h>
#endif //WIN32


//----------------------------------------------------------//
// DEFINES
//----------------------------------------------------------//


//-- Enable for big endian mode
//-- Both PC and Raspberry are little-endian.
//#define SYSMEMORY_BIG_ENDIAN

#if defined(WIN32)

//-- Define this to use safe memory functions, as seen in VC8 and beyond.
#	define SYSMEMORY_USES_SAFE_MEMORY

//-- Define this to use Windows optimised functions like ZeroMemory instead of memset, etc.
//-- In some places, safe memory functions may override this.
#	define SYSMEMORY_USES_WINDOWS_MEMORY_OPTIMISATION

#endif //WIN32


//----------------------------------------------------------//
// GLOBALS
//----------------------------------------------------------//

//----------------------------------------------------------//
// SysMemory::Memcpy
//----------------------------------------------------------//
//-- Description
// Copies a number of bytes from one address to another 
// address
//----------------------------------------------------------//
void* SysMemory::Memcpy(void* pDest, size_t nDestSize, const void* pSrc, size_t nSrcSize)
{
#if defined(SYSMEMORY_USES_SAFE_MEMORY)

    errno_t nError = memcpy_s(pDest, nDestSize, pSrc, nSrcSize);
    if (IS_ZERO(nError))
    {
        return pDest;
    }
    else
    {
        return NULL;
    }

#else
	
    if ( IS_NULL_PTR(pDest)
		|| IS_ZERO(nDestSize) 
		|| IS_NULL_PTR(pSrc) 
		|| IS_ZERO(nSrcSize) )
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

#	endif //SYSMEMORY_USES_WINDOWS_MEMORY_OPTIMISATION

#endif //SYSMEMORY_USES_SAFE_MEMORY
}


//----------------------------------------------------------//
// SysMemory::Memmove
//----------------------------------------------------------//
//-- Description
// Move a number of bytes from one address to another 
// address. Should safely handle cases where src and dest
// overlap in memory.
//----------------------------------------------------------//
void* SysMemory::Memmove(void* pDest, size_t nDestSize, const void* pSrc, size_t nSrcSize)
{
#if defined(SYSMEMORY_USES_SAFE_MEMORY)

    errno_t nError = memmove_s(pDest, nDestSize, pSrc, nSrcSize);
    if (IS_ZERO(nError))
    {
        return pDest;
    }
    else
    {
        return NULL;
    }

#else
	
    if ( IS_NULL_PTR(pDest)
		|| IS_ZERO(nDestSize) 
		|| IS_NULL_PTR(pSrc) 
		|| IS_ZERO(nSrcSize) )
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

#	endif //SYSMEMORY_USES_WINDOWS_MEMORY_OPTIMISATION

#endif //SYSMEMORY_USES_SAFE_MEMORY
}


//----------------------------------------------------------//
// SysMemory::Memset
//----------------------------------------------------------//
//-- Description
// Sets each byte in a block of memory to a specific value
//----------------------------------------------------------//
void* SysMemory::Memset(void* pMem, s8 nValue, size_t nByteCount)
{
    if ( IS_NULL_PTR(pMem)
		|| IS_ZERO(nByteCount) )
    {
        return NULL;
    }

#	if defined(SYSMEMORY_USES_WINDOWS_MEMORY_OPTIMISATION)

		FillMemory(pMem, nByteCount, nValue);
		return pMem;

#	else

		return memset(pMem, nValue, nByteCount);

#	endif //SYSMEMORY_USES_WINDOWS_MEMORY_OPTIMISATION
}


//----------------------------------------------------------//
// SysMemory::Memclear
//----------------------------------------------------------//
//-- Description
// Sets a block of memory to 0
//----------------------------------------------------------//
void* SysMemory::Memclear(void* pMem, size_t nByteCount)
{
    if ( IS_NULL_PTR(pMem)
		|| IS_ZERO(nByteCount) )
    {
        return NULL;
    }

#	if defined(SYSMEMORY_USES_WINDOWS_MEMORY_OPTIMISATION)

		ZeroMemory(pMem, nByteCount);
		return pMem;

#	else

		return memset(pMem, 0, nByteCount);

#	endif //SYSMEMORY_USES_WINDOWS_MEMORY_OPTIMISATION
}


//----------------------------------------------------------//
// SysMemory::Memcmp
//----------------------------------------------------------//
//-- Description
// Compare memory
//----------------------------------------------------------//
s32 SysMemory::Memcmp(const void* pMem1, const void* pMem2, size_t nCount)
{
	return memcmp(pMem1, pMem2, nCount);
}


//----------------------------------------------------------//
// SysMemory::EndianSwapValue
//----------------------------------------------------------//
//-- Description
// Reverses the order of a block of bytes.
// Like switching the bytes in a s32, etc.
//----------------------------------------------------------//
void SysMemory::EndianSwap(void* pMem, size_t nSizeofEntity)
{
	//-- size must be even
	assert( (nSizeofEntity >= 2) && IS_ZERO(nSizeofEntity & 1) );
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
// SysMemory::EndianSwapArray
//----------------------------------------------------------//
//-- Description
// Reverses the bytes for a number of entries in an array
// for example, endian swapping an array of s32, etc.
//----------------------------------------------------------//
void SysMemory::EndianSwapArray(void *pMem, size_t nSizeofEntity, u32 nEntityCount)
{
	//-- size must be even
	assert( (nSizeofEntity >= 2) && IS_ZERO(nSizeofEntity & 1) );
	//-- limit to 8 bytes max.
	assert(nSizeofEntity <= sizeof(u64));

	u8* acBytes = (u8*)pMem;
	for (u32 i = 0; i < nEntityCount; ++i, acBytes += nSizeofEntity)
	{
		SysMemory::EndianSwap((void*)acBytes, nSizeofEntity);
	}
}


//----------------------------------------------------------//
// SysMemory::EndianCopyValue
//----------------------------------------------------------//
//-- Description
// Copies a value, reversing the endian.
//----------------------------------------------------------//
void SysMemory::EndianSwapCopy(void* pDest, const void* pSrc, size_t nSizeofEntity)
{
	//-- size must be even
	assert( (nSizeofEntity >= 2) && IS_ZERO(nSizeofEntity & 1) );
	//-- limit to 8 bytes max.
	assert(nSizeofEntity <= sizeof(u64));

	SysMemory::Memcpy(pDest, nSizeofEntity, pSrc, nSizeofEntity);
	SysMemory::EndianSwap(pDest, nSizeofEntity);
}


//----------------------------------------------------------//
// SysMemory::EndianCopyArray
//----------------------------------------------------------//
//-- Description
// Copies a block of values, reversing the endian of 
// each value.
//----------------------------------------------------------//
void SysMemory::EndianSwapArrayCopy(void* pDest, const void* pSrc, size_t nSizeofEntity, u32 nEntityCount)
{
	//-- size must be even
	assert( (nSizeofEntity >= 2) && IS_ZERO(nSizeofEntity & 1) );
	//-- limit to 8 bytes max.
	assert(nSizeofEntity <= sizeof(u64));

	size_t nTotalSize = nSizeofEntity * nEntityCount;

	SysMemory::Memcpy(pDest, nTotalSize, pSrc, nTotalSize);
	SysMemory::EndianSwapArray(pDest, nSizeofEntity, nEntityCount);
}


//----------------------------------------------------------//
// SysMemory::IsSystemBigEndian
//----------------------------------------------------------//
//-- Description
// Determines if a system is big or little endian
//----------------------------------------------------------//
bool SysMemory::IsSystemBigEndian(void)
{
    s32 nInt = 1;
    s8* pcTemp = (s8*)&nInt;
    return !(*pcTemp);
}


//----------------------------------------------------------//
// SysMemory::Align
//----------------------------------------------------------//
//-- Description
// Given a pointer, aligns the pointer to the nearest byte
// boundary specified and returns a new pointer
//----------------------------------------------------------//
void* SysMemory::Align(const void* pMem, size_t nAlignment)
{
	uintptr_t nAddress = (uintptr_t)pMem;
	--nAlignment;
	nAddress += nAlignment;
	nAddress &= ~nAlignment;

	return (void*)nAddress;
}


//----------------------------------------------------------//
// SysMemory::Pad
//----------------------------------------------------------//
//-- Description
// Given a size, pads the size to the nearest byte
// boundary specified and returns a new size
//----------------------------------------------------------//
size_t SysMemory::Pad(size_t nSize, size_t nAlignment)
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
