#ifndef _SIMPLE_BUFFER_H_
#define _SIMPLE_BUFFER_H_
#pragma once

//----------------------------------------------------------//
// SIMPLEBUFFER.H
//----------------------------------------------------------//
//-- Description
// Very basic buffer template.
//----------------------------------------------------------//


#include "ISimpleBuffer.h"
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
class CSimpleBuffer : public ISimpleBuffer
{
	public:
	
		CSimpleBuffer()
		{
			Clear();
		}

		virtual ~CSimpleBuffer()
		{
			Clear();
		}

		virtual u8* Buffer(void)
		{
			return m_Buffer;
		}
		virtual const u8* ConstBuffer(void) const
		{
			return m_Buffer;
		}
		virtual size_t Size(void) const
		{
			return S;
		}
		virtual size_t UsedSize(void) const
		{
			return m_nFilledSize;
		}
		virtual size_t UnusedSize(void) const
		{
			return Size() - UsedSize();
		}

		virtual ISimpleBuffer::Error::Enum GetError() const
		{
			return m_eError;
		}

		virtual void Clear(void)
		{
			m_nFilledSize = 0;
			m_eError = Error::Ok;
		}

		virtual bool IsEmpty(void)
		{
			return IS_TRUE(0 == m_nFilledSize);
		}

		virtual u8* InsHead(const u8* pIn, size_t nInSize)
		{
			m_eError = Error::Ok;

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

		virtual u8* InsTail(const u8* pIn, size_t nInSize)
		{
			m_eError = Error::Ok;

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
							m_eError = Error::CopyFailed;

							SysMemory::Memclear(pBuf, nInSize);
						}
					}
					else
					{
						//-- Nothing to copy.
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

		virtual u8* StripHead(u8* pOut, size_t nOutSize)
		{
			m_eError = Error::Ok;

			if (nOutSize > 0)
			{
				if (nOutSize <= m_nFilledSize)
				{
					if (IS_PTR(pOut))
					{
						if (IS_PTR(SysMemory::Memcpy(pOut, nOutSize, m_Buffer, nOutSize)))
						{
							//-- Copy success.
						}
						else
						{
							//-- Copy failed.
							m_eError = Error::CopyFailed;
						}
					}

					if (IS_PTR(SysMemory::Memmove(m_Buffer, S, m_Buffer + nOutSize, S - nOutSize)))
					{
						//-- Success.
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

		virtual u8* StripTail(u8* pOut, size_t nOutSize)
		{
			m_eError = Error::Ok;

			if (nOutSize > 0)
			{
				if (nOutSize <= m_nFilledSize)
				{
					if (IS_PTR(pOut))
					{
						if (IS_PTR(SysMemory::Memcpy(pOut, nOutSize, m_Buffer + m_nFilledSize - nOutSize, nOutSize)))
						{
							//-- Copy success.
						}
						else
						{
							//-- Copy failed.
							m_eError = Error::CopyFailed;
						}
					}

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
		Error::Enum				m_eError;
};


//----------------------------------------------------------//
// EXTERNALS
//----------------------------------------------------------//

//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//

#endif //_SIMPLE_BUFFER_H_
