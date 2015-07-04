#ifndef _WINSYS_ERRORS_H_
#define _WINSYS_ERRORS_H_
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

#define WINSYS_INIT_FAIL								(0x80000100)

#define WINSYS_CONFIG_FAIL								(0x80000200)
#define WINSYS_CONFIG_CANCEL							(0x80000201)
#define WINSYS_CONFIG_DIALOG_FAIL						(0x80000202)
#define WINSYS_CONFIG_INI_NOT_LOADED					(0x80000210)
#define WINSYS_CONFIG_INI_NOT_SAVED						(0x80000211)
#define WINSYS_CONFIG_INPUT_NOT_PROCESSED				(0x80000212)
#define WINSYS_CONFIG_INPUT_CURSOR_OUT_OF_BOUNDS		(0x80000213)
#define WINSYS_CONFIG_ROLLUPCONTAINER_FAIL				(0x80000220)
#define WINSYS_CONFIG_ROLLUPCONTAINER_NOT_FOUND			(0x80000221)
#define WINSYS_CONFIG_ROLLUPCONTAINER_ADDROLLUP_FAIL	(0x80000222)

#define WINSYS_WINDOW_FAIL								(0x80000300)
#define WINSYS_WINDOW_FULLSCREEN_SWITCH_FAILED			(0x80000301)
#define WINSYS_WINDOW_BAD_WIDTH_OR_HEIGHT				(0x80000302)
#define WINSYS_WINDOW_CLASS_NOT_REGISTERED				(0x80000305)
#define WINSYS_WINDOW_HWND_NOT_CREATED					(0x80000306)
#define WINSYS_WINDOW_HDC_NOT_CREATED					(0x80000307)
#define WINSYS_WINDOW_PIXELFORMAT_NOT_FOUND				(0x80000308)
#define WINSYS_WINDOW_PIXELFORMAT_NOT_SET				(0x80000309)
#define WINSYS_WINDOW_HRC_NOT_CREATED					(0x8000030a)				
#define WINSYS_WINDOW_HRC_NOT_ACTIVATED					(0x8000030b)

#define WINSYS_TIMER_FPS_TARGET_NOT_REACHED				(0x80000400)


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

#endif //_WINSYS_ERRORS_H_
