#ifndef _WINSYS_GL_WINDOW_H_
#define _WINSYS_GL_WINDOW_H_

//----------------------------------------------------------//
// WINSYSGLWINDOW.H
//----------------------------------------------------------//
//-- Description			
//----------------------------------------------------------//


#include "WinSysIncludes.h"


//----------------------------------------------------------//
// DEFINES
//----------------------------------------------------------//

//----------------------------------------------------------//
// ENUMS
//----------------------------------------------------------//

//----------------------------------------------------------//
// FORWARD REFERENCES
//----------------------------------------------------------//


//class CWinSysFrameTimer;


//----------------------------------------------------------//
// STRUCTS
//----------------------------------------------------------//

//----------------------------------------------------------//
// CLASSES
//----------------------------------------------------------//


class CWinSysGLWindow
{
	public:

		struct WindowMode
		{
			enum Enum 
			{
				Windowed = 0,
				Borderless,
				Fullscreen
			};
		};

		CWinSysGLWindow();
		~CWinSysGLWindow();

		s32				InitialiseFullscreenMode(s32 nWidth, s32 nHeight, s32 nBPP);
		s32				InitialiseWindowedMode(s32 nWidth, s32 nHeight, s32 nBPP);
		s32				InitialiseBorderlessWindowMode(s32 nBPP);
		s32				Shutdown(void);

		void			SwapBuffers(void) const;

		static LRESULT CALLBACK EventProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

		void			SetWindowTitle(const s8* strBuffer);

		WindowMode::Enum GetWindowMode(void) const;

		void			SetActive(bool bActive);
		bool			IsActive(void) const;

	private:

		s32				ActivateFullscreenDisplay(s32 nWidth, s32 nHeight, s32 nBPP);
		s32				RevertFullscreenDisplay(void);		
		s32				CreateGLWindow(s32 nWidth, s32 nHeight, s32 nBPP, DWORD dwStyle, DWORD dwExStyle);
		s32				DestroyGLWindow(void);

		HINSTANCE		m_hInstance;
		HWND			m_hWnd;
		HDC				m_hDC;
		HGLRC			m_hRC;

//		CWinSysFrameTimer m_FrameTimer;

		WindowMode::Enum m_eWindowMode;
		bool			m_bRegisteredClass;
		bool			m_bFulllscreenDisplayActivated;

		bool			m_bActiveWindow;
};


//----------------------------------------------------------//
// EXTERNALS
//----------------------------------------------------------//


extern CWinSysGLWindow gWinSysGLWindow;


//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//

#endif //_WINSYS_GL_WINDOW_H_