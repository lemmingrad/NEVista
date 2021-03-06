#ifndef _SYS_TIME_H_
#define _SYS_TIME_H_
#pragma once

//----------------------------------------------------------//
// SYSTIME.H
//----------------------------------------------------------//
//-- Description
// Wrapper for time functions
//----------------------------------------------------------//


#include "Types.h"
#include <ctime>


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


class SysTime
{
	public:

		static const size_t	CTIME_BUFFER_SIZE = 26;

		static time_t Time(time_t* pTime);
		static s8* Ctime(s8* strDest, size_t nDestSize, const time_t* pTime);

		static u64 GetTicks();
		static u64 GetTickFrequency();

		static u64 GetSystemTime();

	private:

		SysTime();
		~SysTime();
};


//----------------------------------------------------------//
// EXTERNALS
//----------------------------------------------------------//

//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//

#endif
