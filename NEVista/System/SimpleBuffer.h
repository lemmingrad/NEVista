#ifndef _SIMPLE_BUFFER_H_
#define _SIMPLE_BUFFER_H_
#pragma once

//----------------------------------------------------------//
// SIMPLEBUFFER.H
//----------------------------------------------------------//
//-- Description
// Very basic buffer template.
//----------------------------------------------------------//


#include "Types.h"
#include "SysMemory.h"


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


template <size_t S>
class CSimpleBuffer
{
	public:

		CSimpleBuffer()
		{
			Clear();
		}

		~CSimpleBuffer()
		{
			Clear();
		}

		u8* Buffer(void)
		{
			return m_Buffer;
		}
		const u8* ConstBuffer(void) const
		{
			return m_Buffer;
		}
		size_t Size(void) const
		{
			return S;
		}
		size_t UsedSize(void) const
		{
			return m_nFilledSize;
		}
		size_t UnusedSize(void) const
		{
			return Size() - UsedSize();
		}

		void Clear(void)
		{
			m_nFilledSize = 0;
		}

		bool IsEmpty(void)
		{
			return IS_TRUE(m_nFilledSize > 0);
		}

		u8* InsHead(const u8* pIn, size_t nInSize)
		{
			size_t nRemaining = S - m_nFilledSize;
			if (nInSize <= nRemaining)
			{
				if (IS_PTR(SysMemory::Memmove(m_Buffer + nInSize, S - nInSize, m_Buffer, m_nFilledSize)))
				{
					if (IS_PTR(pIn))
					{
						if (IS_PTR(SysMemory::Memcpy(m_Buffer, nInSize, pIn, nInSize)))
						{
							//-- Copy success.
						}
						else
						{
							//-- Copy failed.
						}
					}
					m_nFilledSize += nInSize;
					return m_Buffer;
				}
			}

			//-- Failed.
			return NULL;
		}

		u8* InsTail(const u8* pIn, size_t nInSize)
		{
			if (nInSize > 0)
			{
				size_t nRemaining = S - m_nFilledSize;
				if (nInSize <= nRemaining)
				{
					u8* pBuf = m_Buffer + m_nFilledSize;
					if (IS_PTR(pIn))
					{
						if (IS_PTR(SysMemory::Memcpy(pBuf, nRemaining, pIn, nInSize)))
						{
							//-- Copy success.
						}
						else
						{
							//-- Copy failed.
						}
					}
					m_nFilledSize += nInSize;
					return pBuf;
				}
			}

			//-- Failed.
			return NULL;
		}

		u8* StripHead(size_t nOutSize)
		{
			if (nOutSize > 0)
			{
				if (nOutSize <= m_nFilledSize)
				{
					if (IS_PTR(SysMemory::Memmove(m_Buffer, S - nOutSize, m_Buffer + nOutSize, m_nFilledSize)))
					{
						//-- Success.
						m_nFilledSize -= nOutSize;
						return m_Buffer;
					}
				}
			}

			//-- Failed.
			return NULL;
		}

		u8* StripTail(size_t nOutSize)
		{
			if (nOutSize > 0)
			{
				if (nOutSize <= m_nFilledSize)
				{
					//-- Success.
					m_nFilledSize -= nOutSize;
					return m_Buffer;
				}
			}

			//-- Failed.
			return NULL;
		}

	private:

		u8			m_Buffer[S];
		size_t		m_nFilledSize;
};


//----------------------------------------------------------//
// EXTERNALS
//----------------------------------------------------------//

//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//

#endif //_SIMPLE_BUFFER_H_
