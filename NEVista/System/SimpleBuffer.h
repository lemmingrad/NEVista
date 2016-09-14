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

		struct Error
		{
			enum Enum
			{
				MoveFailed = -6,
				CopyFailed = -5,
				BadParameter = -4,
				WouldUnderflow = -3,
				WouldOverflow = -2,
				Fail = -1,
				Ok = 0
			};
		}; 
		
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

		typename Error::Enum GetError() const
		{
			return m_eError;
		}

		void Clear(void)
		{
			m_nFilledSize = 0;
			m_eError = Error::Ok;
		}

		bool IsEmpty(void)
		{
			return IS_TRUE(0 == m_nFilledSize);
		}

		u8* InsHead(const u8* pIn, size_t nInSize)
		{
			if (nInSize > 0)
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
								m_eError = Error::Ok;
							}
							else
							{
								//-- Copy failed.
								m_eError = Error::CopyFailed;

								SysMemory::Memclear(m_Buffer, nInSize);
							}
						}
						else
						{
							//-- Nothing to copy.
							m_eError = Error::Ok;

							SysMemory::Memclear(m_Buffer, nInSize);
						}

						m_nFilledSize += nInSize;
						return m_Buffer;
					}
					else
					{
						m_eError = Error::MoveFailed;
					}
				}
				else
				{
					m_eError = Error::WouldOverflow;
				}
			}
			else
			{
				m_eError = Error::BadParameter;
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
							m_eError = Error::Ok;
						}
						else
						{
							//-- Copy failed.
							m_eError = Error::CopyFailed;

							SysMemory::Memclear(pBuf, nInSize);
						}
					}
					else
					{
						//-- Nothing to copy.
						m_eError = Error::Ok;

						SysMemory::Memclear(pBuf, nInSize);
					}

					m_nFilledSize += nInSize;
					return pBuf;
				}
				else
				{
					m_eError = Error::WouldOverflow;
				}
			}
			else
			{
				m_eError = Error::BadParameter;
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
						m_eError = Error::Ok;

						m_nFilledSize -= nOutSize;
						return m_Buffer;
					}
					else
					{
						m_eError = Error::MoveFailed;
					}
				}
				else 
				{
					m_eError = Error::WouldUnderflow;
				}
			}
			else
			{
				m_eError = Error::BadParameter;
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
					m_eError = Error::Ok;

					m_nFilledSize -= nOutSize;
					return m_Buffer;
				}
				else
				{
					m_eError = Error::WouldUnderflow;
				}
			}
			else
			{
				m_eError = Error::BadParameter;
			}

			//-- Failed.
			return NULL;
		}

	private:

		u8						m_Buffer[S];
		size_t					m_nFilledSize;
		typename Error::Enum	m_eError;
};


//----------------------------------------------------------//
// EXTERNALS
//----------------------------------------------------------//

//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//

#endif //_SIMPLE_BUFFER_H_
