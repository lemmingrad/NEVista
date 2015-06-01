#ifndef _WINSYSERRORS_H_
#define _WINSYSERRORS_H_
#pragma once

//----------------------------------------------------------//
// WINSYSERRORS.H
//----------------------------------------------------------//
//-- Description	
// Return codes and errors for WinSys
//----------------------------------------------------------//

//----------------------------------------------------------//
// DEFINES
//----------------------------------------------------------//


#define WINSYS_OK										(0x00000000)

#define WINSYS_FAIL										(0x80000000)
#define WINSYS_QUIT										(0x80000001)

#define WINSYS_INIT_FAIL								(0x80002000)

#define WINSYS_CONFIG_FAIL								(0x80004000)
#define WINSYS_CONFIG_CANCEL							(0x80004001)
#define WINSYS_CONFIG_DIALOG_FAIL						(0x80004002)
#define WINSYS_CONFIG_INI_NOT_LOADED					(0x80004100)
#define WINSYS_CONFIG_INI_NOT_SAVED						(0x80004101)
#define WINSYS_CONFIG_INPUT_NOT_PROCESSED				(0x80004102)
#define WINSYS_CONFIG_INPUT_CURSOR_OUT_OF_BOUNDS		(0x80004103)
#define WINSYS_CONFIG_ROLLUPCONTAINER_FAIL				(0x80004200)
#define WINSYS_CONFIG_ROLLUPCONTAINER_NOT_FOUND			(0x80004201)
#define WINSYS_CONFIG_ROLLUPCONTAINER_ADDROLLUP_FAIL	(0x80004202)

#define WINSYS_WINDOW_FAIL								(0x80005000)
#define WINSYS_WINDOW_FULLSCREEN_SWITCH_FAILED			(0x80005001)
#define WINSYS_WINDOW_CLASS_NOT_REGISTERED				(0x80005100)
#define WINSYS_WINDOW_HWND_NOT_CREATED					(0x80005101)
#define WINSYS_WINDOW_HDC_NOT_CREATED					(0x80005102)
#define WINSYS_WINDOW_PIXELFORMAT_NOT_FOUND				(0x80005103)
#define WINSYS_WINDOW_PIXELFORMAT_NOT_SET				(0x80005104)
#define WINSYS_WINDOW_HRC_NOT_CREATED					(0x80005105)				
#define WINSYS_WINDOW_HRC_NOT_ACTIVATED					(0x80005106)

#define WINSYS_TIMER_FPS_TARGET_NOT_REACHED				(0x80006000)


//----------------------------------------------------------//
// ENUMS
//----------------------------------------------------------//

//----------------------------------------------------------//
// STRUCTS
//----------------------------------------------------------//

//----------------------------------------------------------//
// CLASSES
//----------------------------------------------------------//

//----------------------------------------------------------//
// EXTERNALS
//----------------------------------------------------------//

//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//

#endif //_WINSYSERRORS_H_
