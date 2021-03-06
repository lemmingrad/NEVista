#ifndef _WINSYS_MAIN_H_
#define _WINSYS_MAIN_H_
#pragma once

//----------------------------------------------------------//
// WINSYSMAIN.H
//----------------------------------------------------------//
//-- Description			
//----------------------------------------------------------//


#include "Types.h"
#include "Log.h"
#include <windows.h>


//----------------------------------------------------------//
// DEFINES
//----------------------------------------------------------//

//----------------------------------------------------------//
// ENUMS
//----------------------------------------------------------//


struct WinSysInitState
{
	enum Enum
	{
		Finished = 0,
		ResolvingMode, 
		ShowConfigDialog,
		CreateWindow
	};
};


//----------------------------------------------------------//
// STRUCTS
//----------------------------------------------------------//

//----------------------------------------------------------//
// CLASSES
//----------------------------------------------------------//

//----------------------------------------------------------//
// EXTERNALS
//----------------------------------------------------------//


int WinSys_Main(LPSTR lpCmdLine);
s32	WinSys_Initialise(void);
s32	WinSys_Shutdown(void);
s32	WinSys_Update(void);


//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//

#endif //_WINSYS_MAIN_