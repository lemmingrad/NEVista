//----------------------------------------------------------//
// SYSDEBUGLOG.CPP
//----------------------------------------------------------//
//-- Description			
// Debug log.
//----------------------------------------------------------//


#include "SysDebugLog.h"
#include "Types.h"
#include "Log.h"
#if defined(WIN32)
#	include <windows.h>
#endif


//----------------------------------------------------------//
// DEFINES
//----------------------------------------------------------//

//----------------------------------------------------------//
// GLOBALS
//----------------------------------------------------------//


CLog gDebugLog;


s32 SysDebugPrintf(const s8* strFormatting, ...)
{
	FixedString<1024> workBuffer;

	va_list ArgList;
	va_start(ArgList, strFormatting);
	s32 nRet = SysString::Vsprintf(workBuffer.Buffer(), workBuffer.Size(), strFormatting, ArgList);
	va_end(ArgList);

	#if defined(WIN32)
		
		OutputDebugString(workBuffer.ConstBuffer());
		return nRet;

	#else

		return printf(workBuffer.ConstBuffer());

	#endif
}


//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//
