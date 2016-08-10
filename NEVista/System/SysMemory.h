#ifndef _SYS_MEMORY_H_
#define _SYS_MEMORY_H_
#pragma once

//----------------------------------------------------------//
// SYSMEMORY.H
//----------------------------------------------------------//
//-- Description
// Wrapper for memory access.
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
// CLASSES
//----------------------------------------------------------//


class SysMemory
{
	public:

		//-- Overrides for memory operations
		static void* Memcpy(void* pDest, size_t nDestSize, const void* pSrc, size_t nSrcSize);
		static void* Memmove(void* pDest, size_t nDestSize, const void* pSrc, size_t nSrcSize);
		static void* Memset(void* pMem, s8 nValue, size_t nByteCount);
		static void* Memclear(void* pMem, size_t nByteCount);
		static s32	 Memcmp(const void* pMem1, const void* pMem2, size_t nCount);

		//-- Endian
		static bool	 IsSystemBigEndian(void);
		static void	 EndianSwap(void* pMem, size_t nSizeofEntity);
		static void  EndianSwapArray(void* pMem, size_t nSizeofEntity, u32 nEntityCount);
		static void	 EndianSwapCopy(void* pDest, const void* pSrc, size_t nSizeofEntity);
		static void  EndianSwapArrayCopy(void* pDest, const void* pSrc, size_t nSizeofEntity, u32 nEntityCount);
		
		//-- Alignment and padding
		static void* Align(const void* pMem, size_t nAlignment);
		static size_t Pad(size_t nSize, size_t nAlignment);

	private:
		
		SysMemory();
		~SysMemory();
};


//----------------------------------------------------------//
// EXTERNALS
//----------------------------------------------------------//

//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//

#endif