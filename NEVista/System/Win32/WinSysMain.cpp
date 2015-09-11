//----------------------------------------------------------//
// WINSYSMAIN.CPP
//----------------------------------------------------------//
//-- Description			
// This is our 'main' function for Windows
//----------------------------------------------------------//


#include "WinSysIncludes.h"
#include "Types.h"
#include "SysSocket.h"
#include "SysDebugLog.h"
#include "Win32/Game.h"


//----------------------------------------------------------//
// DEFINES
//----------------------------------------------------------//

//----------------------------------------------------------//
// GLOBALS
//----------------------------------------------------------//

//----------------------------------------------------------//
// WinSys_Initialise
//----------------------------------------------------------//
//-- Description			
// Initialises Windows System, ie, decides when to show
// Config Dialog and GL Window
//----------------------------------------------------------//
s32 WinSys_Initialise(void)
{
	s32 nReturnStatus = WINSYS_OK;

	//-- Initialise a debug log
	if (IS_FALSE(gDebugLog.Initialise("DebugLog.txt")))
	{
		MessageBox(NULL, "Debug Log cannot be created. Logging disabled.", TEXT(Game_Title()), MB_OK|MB_ICONEXCLAMATION);
	}
	gDebugLog.Printf("NEVista Title: %s, Version: %s", Game_Title(), Game_Version());

/*
	//-- Initialise Mem Manager
	if (IS_FALSE(gMemMgr.Initialise())
	{
		//-- Failed to initialise memory manager
		return WINSYS_INIT_MEMMGR_FAIL;
	}

	//-- Initialise File Manager
	if (IS_FALSE(gFileMgr.Initialise())
	{
		//-- Failed to initialise file manager
		return WINSYS_INIT_FILEMGR_FAIL;
	}
	
	//-- Initialise Input handler
	if (IS_FALSE(gInput.Initialise())
	{
		//-- Failed to initialise input handler
		return WINSYS_INIT_INPUT_FAIL;	
	}
*/
	
	//-- Initialise system sockets
	SysSocket::SystemInitialise();

	CWinSysConfigDialog configDialog;
	nReturnStatus = configDialog.Initialise(Game_Title(), Game_Version());
	if (WINSYS_OK != nReturnStatus)
	{
		//-- Failed to initialise config dialog
		return nReturnStatus;
	}
	
	WinSysInitState::Enum eInitState = WinSysInitState::ResolvingMode;
	while (WinSysInitState::Finished != eInitState)
	{
		switch (eInitState)
		{
			default:
			break;

			case WinSysInitState::ResolvingMode:
			{
				if (IS_TRUE(configDialog.ShowAtStartup()))
				{
					eInitState = WinSysInitState::ShowConfigDialog;
				}
				else
				{
					eInitState = WinSysInitState::CreateWindow;
				}
			}
			break;

			case WinSysInitState::ShowConfigDialog:
			{
				//-- creates the config dialog - the dialog will block here.
				nReturnStatus = configDialog.Show();
				if (WINSYS_OK == nReturnStatus)
				{
					//-- Success
					eInitState = WinSysInitState::CreateWindow;
				}
				else
				{
					//-- Config dialog failed or was closed
					eInitState = WinSysInitState::Finished;
				}
			}
			break;

			case WinSysInitState::CreateWindow:
			{
				s32 nWidth = configDialog.GetDesiredResolutionWidth();
				s32 nHeight = configDialog.GetDesiredResolutionHeight();
				s32 nBPP = configDialog.GetDesiredBPPValue();

				switch (configDialog.GetDesiredWindowMode())
				{
					case CWinSysConfigDialog::DesiredWindowMode::Fullscreen:
					{
						if (WINSYS_OK == gWinSysGLWindow.InitialiseFullscreenMode(nWidth, nHeight, nBPP))
						{
							//-- Success
							eInitState = WinSysInitState::Finished;
						}
						else
						{
							//-- Failed to create window
							eInitState = WinSysInitState::ResolvingMode;
						}
					}
					break;

					case CWinSysConfigDialog::DesiredWindowMode::Borderless:
					{
						if (WINSYS_OK == gWinSysGLWindow.InitialiseBorderlessWindowMode(nBPP))
						{
							//-- Success
							eInitState = WinSysInitState::Finished;
						}
						else
						{
							//-- Failed to create window
							eInitState = WinSysInitState::ResolvingMode;
						}
					}
					break;

					default:
					{
						if (WINSYS_OK == gWinSysGLWindow.InitialiseWindowedMode(nWidth, nHeight, nBPP))
						{
							//-- Success
							eInitState = WinSysInitState::Finished;
						}
						else
						{
							//-- Failed to create window
							eInitState = WinSysInitState::ResolvingMode;
						}
					}
					break;
				}
			}
			break;
		}
	}

	configDialog.Shutdown();

	return nReturnStatus;
}


//----------------------------------------------------------//
// WinSys_Shutdown
//----------------------------------------------------------//
//-- Description			
// Shuts down all the internal system specific parts that are 
// not part of Game_Shutdown.
//----------------------------------------------------------//
s32 WinSys_Shutdown(void)
{
	//-- Shutdown the GL window if it exists
	gWinSysGLWindow.Shutdown();
	//-- and sockets
	SysSocket::SystemShutdown();
	//-- and Input
//	gInput.Shutdown();
	//-- and the timer
//	gWinSysTimer.Shutdown();
	//-- and the File manager
//	gFileMgr.Shutdown();
	//-- and the memory manager
//	gMemMgr.Shutdown();

	//-- and the debug log
	gDebugLog.Shutdown();

	return WINSYS_OK;
}


//----------------------------------------------------------//
// WinSys_Update
//----------------------------------------------------------//
//-- Description			
// Updates the system components
//----------------------------------------------------------//
s32 WinSys_Update(void)
{
//	gInput.RemoveEvents();

	MSG msg;
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) > 0)
	{
		if (msg.message == WM_QUIT)
		{
			return WINSYS_QUIT;
		}

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	
//	gWinSysTimer.Update();
//	gMemMgr.Update();
//	gFileMgr.Update();

	return WINSYS_OK;
}


//----------------------------------------------------------//
// WinSys_Main
//----------------------------------------------------------//
//-- Description			
// The main function.
//----------------------------------------------------------//
int WinSys_Main(LPSTR lpCmdLine) 
{
	if (WINSYS_OK != WinSys_Initialise())
	{
		WinSys_Shutdown();
		return 0;
	}

	//-- Attempt to Initialise the game
	if (IS_FALSE(Game_Initialise()))
	{
		//-- Initialise failed, so shut down nicely
		Game_Shutdown();
		WinSys_Shutdown();
		return 0;
	}

	//-- Main game loop
	while (true)
	{
		if (WINSYS_OK != WinSys_Update())
		{
			//-- Loop until the message handler returns something not OK
			break;
		}

		Game_Main();

		gWinSysGLWindow.SwapBuffers();

//		while (IS_FALSE(m_FrameTimer.LockFrameRate(60.0f)))
//		{
//			//-- Todo more stuff while waiting for VSync.
//			gMemMgr.Update();
//			gFileMgr.Update();
//		}
	}
	
	//-- Shut down
	Game_Shutdown();
	WinSys_Shutdown();
	return 0;
}


//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//
