//----------------------------------------------------------//
// SYSTIME.CPP
//----------------------------------------------------------//
//-- Description
// Wrapper for time functions
//----------------------------------------------------------//


#include "SysTime.h"
#include "Types.h"
#include <ctime>
#include "SysString.h"
#if defined(WIN32)
#	include <windows.h>
#endif


//----------------------------------------------------------//
// DEFINES
//----------------------------------------------------------//


#if defined(WIN32)

//-- Define this to use safer time functions in VC8 and beyond
//-- Will override ctime_r if defined below.
#	define SYSTIME_USES_SAFE_TIME

#elif defined(LINUX)

//-- Define this to use re-entrant ctime_r() instead of ctime()
//-- Ignored if using the safe time functions defined above.
//#	define SYSTIME_USES_CTIME_R

#endif


//----------------------------------------------------------//
// GLOBALS
//----------------------------------------------------------//

//----------------------------------------------------------//
// SysTime::Time
//----------------------------------------------------------//
//-- Description
// Wrapper around time function
//----------------------------------------------------------//
time_t SysTime::Time(time_t* pTime)
{
	return time(pTime);
}


//----------------------------------------------------------//
// SysTime::Ctime
//----------------------------------------------------------//
//-- Description
// Wrapper around ctime function
//----------------------------------------------------------//
s8* SysTime::Ctime(s8* strDest, size_t nDestSize, const time_t* pTime)
{
#if defined(SYSTIME_USES_SAFE_TIME)

	errno_t nError = ctime_s(strDest, nDestSize, pTime);
	if (IS_ZERO(nError))
	{
		return strDest;
	}
	else
	{
		return NULL;
	}

#else

	if ( IS_NULL_PTR(strDest) 
		|| (nDestSize < SYSTIME_BUFFER_SIZE)
		|| (IS_NULL_PTR(pTime)) )
	{
		//-- Be consistent with the behavior of ctime_s.
		return NULL;
	}

#	if defined(SYSTIME_USES_CTIME_R)

		return ctime_r(pTime, strDest);

#	else
	
		return SysString::Strncpy(strDest, nDestSize, ctime(pTime), nDestSize);

#	endif //SYSTIME_USES_CTIME_R

#endif //SYSTIME_USES_SAFE_TIME
}


//----------------------------------------------------------//
// SysTime::GetTicks
//----------------------------------------------------------//
//-- Description
// Wrapper around function to get system ticks.
//----------------------------------------------------------//
u64 SysTime::GetTicks()
{
#if defined(WIN32)

	LARGE_INTEGER ticks;
	if (QueryPerformanceCounter(&ticks))
	{
		//-- Performance counter is available. Return the count.
		return ticks.QuadPart;
	}

	//-- Performance counter is not available. Fallback to using GetTickCount(), which works in milliseconds.
	return (u64)GetTickCount();

#else

	//-- clock_gettime returns seconds + nanoseconds, but we're going to convert to microseconds.
	timespec tv;
	if (0 == clock_gettime(CLOCK_MONOTONIC, &tv))
	{
		return (u64)tv.tv_sec * 1000000 + (u64)tv.tv_nsec / 1000;
	}

	//-- CLOCK_MONOTONIC not supported.
	return 0;

#endif
}


//----------------------------------------------------------//
// SysTime::GetTickFrequency
//----------------------------------------------------------//
//-- Description
// Wrapper around function to get system tick frequency.
//----------------------------------------------------------//
u64 SysTime::GetTickFrequency()
{
#if defined(WIN32)

	LARGE_INTEGER ticksPerSec;
	if (QueryPerformanceFrequency(&ticksPerSec))
	{
		//-- Performance counter is available. Return the frequency.
		return ticksPerSec.QuadPart;
	}

	//-- Performance counter is not available. Fallback to using GetTickCount(), which works in milliseconds.
	return 1000;

#else

	//-- clock_gettime returns seconds + nanoseconds, but we're going to convert to microseconds.
	return 1000000;

#endif
}


//----------------------------------------------------------//
// SysTime::GetSystemTime
//----------------------------------------------------------//
//-- Description
// Wrapper around function to get system time UTC
//----------------------------------------------------------//
u64 SysTime::GetSystemTime()
{
#if defined(WIN32)

	FILETIME fileTime;
	GetSystemTimeAsFileTime(&fileTime);

	//-- GetSystemTimeAsFileTime returns 1/100 nanoseconds, but we're going to convert to microseconds.
	u64 timeValue = u64(fileTime.dwHighDateTime) << 32;
	timeValue |= u64(fileTime.dwLowDateTime);
	//-- Change epoch from 00:00:00 1 Jan 1601 (Windows time) to 00:00:00 1 Jan 1970 (Unix time)
	timeValue -= 116444736000000000LL; 
	//-- Convert from 1 tick per 100 nanoseconds to 1 tick per microsecond
	timeValue /= 10;
	return timeValue;

#else

	timespec tv;
	if (0 == clock_gettime(CLOCK_REALTIME, &tv))
	{
		//-- clock_gettime returns seconds + nanoseconds, but we're going to convert to microseconds.
		return (u64)tv.tv_sec * 1000000 + (u64)tv.tv_nsec / 1000;
	}

	//-- CLOCK_REALTIME not supported.
	return 0;

#endif
}




//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//
