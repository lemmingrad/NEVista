#ifndef _SYS_SMARTPTR_H_
#define _SYS_SMARTPTR_H_
#pragma once

//----------------------------------------------------------//
// SYSSMARTPTR.H
//----------------------------------------------------------//
//-- Description
// Custom smart pointer implementation
// (because my c++ doesn't have std::shared_ptr...)
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


class SysSmartPtrRefCounter
{
	public:

		SysSmartPtrRefCounter()
		: m_nCount(0)
		{
		}

		~SysSmartPtrRefCounter()
		{
		}

		void AddRef(void)
		{
			//-- Increment ref counter
			m_nCount++;
		}

		u32 Release(void)
		{
			//-- Decrement ref counter
			if (m_nCount > 0)
			{
				return --m_nCount;
			}
			return 0;
		}

		u32	GetCount(void)
		{
			return m_nCount;
		}

	private:

		u32		m_nCount;
};


template <class T>
class SysSmartPtr
{
	public:

		//-- Basic constructor
		//-- Create a new ref counter and increment it.
		SysSmartPtr()
		{
			Acquire(new SysSmartPtrRefCounter(), NULL);
		}

		//-- Sensible constructor
		//-- Create new ref counter and increment it, and actually have a data pointer to keep track of.
		SysSmartPtr(T* pPtr)
		{
			Acquire(new SysSmartPtrRefCounter(), pPtr);
		}

		//-- Copy constructor
		//-- Copies the internal pointers and increments the ref counter.
		SysSmartPtr(const SysSmartPtr<T>& sp)
		{
			Acquire(sp.m_pRefCounter, sp.m_pPtr);
		}

		//-- Destructor
		//-- Delete data if ref counter drops to 0.
		~SysSmartPtr()
		{
			Release();
		}

		T& operator*()
		{
			return *m_pPtr;
		}

		T* operator->()
		{
			return m_pPtr;
		}

		//-- Assignment operator
		SysSmartPtr<T>& operator=(const SysSmartPtr<T>& sp)
		{
			if (this != &sp)
			{
				Release();
				Acquire(sp.m_pRefCounter, sp.m_pPtr);
			}

			return *this;
		}

		//-- IsUnique
		//-- Is there only one reference?
		bool IsUnique(void)
		{
			if (IS_PTR(m_pRefCounter))
			{
				return (1 == m_pRefCounter->GetCount());
			}
			return false;
		}
	
	private:

		SysSmartPtrRefCounter*		m_pRefCounter;
		T*							m_pPtr;

		void Acquire(SysSmartPtrRefCounter* pCounter, T* pPtr) 
		{
			m_pPtr = pPtr;
			m_pRefCounter = pCounter;
			assert(m_pRefCounter);
			if (IS_PTR(m_pRefCounter))
			{
				m_pRefCounter->AddRef();
			}
		}

		void Release(void) 
		{	
			if (IS_PTR(m_pRefCounter))
			{
				if (IS_ZERO(m_pRefCounter->Release()))
				{
					SAFE_DELETE(m_pPtr);
					SAFE_DELETE(m_pRefCounter);
				}
			}
		}
};


//----------------------------------------------------------//
// EXTERNALS
//----------------------------------------------------------//

//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//

#endif //_SYS_SMARTPTR_H_
