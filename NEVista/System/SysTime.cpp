//----------------------------------------------------------//
// SYSTIME.CPP
//----------------------------------------------------------//
//-- Description
// Wrapper for time functions
//----------------------------------------------------------//


#include "Types.h"

#include "SysTime.h"


//----------------------------------------------------------//
// DEFINES
//----------------------------------------------------------//


//-- Define this to use safer time functions in VC8 and beyond
#define SYSTIME_USES_SAFE_TIME


//----------------------------------------------------------//
// GLOBALS
//----------------------------------------------------------//

//----------------------------------------------------------//
// Time::Ctime
//----------------------------------------------------------//
//-- Description
// Wrapper around ctime function
//----------------------------------------------------------//
s8* Time::Ctime(s8* strDest, size_t nDestSize, const time_t* pTime)
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
		|| IS_ZERO(nDestSize) )
	{
		//-- Be consistent with the behavior of ctime_s.
		return NULL;
	}

	String::Strcpy(strDest, nDestSize, ctime(pTime), nDestSize);
	return strDest;

#endif
}


//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//
