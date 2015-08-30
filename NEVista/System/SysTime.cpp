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
		|| (nDestSize < 26) 
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
// EOF
//----------------------------------------------------------//
