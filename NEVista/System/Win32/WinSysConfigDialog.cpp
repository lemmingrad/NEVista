//----------------------------------------------------------//
// WINSYSCONFIGDIALOG.CPP
//----------------------------------------------------------//
//-- Description			
// Config dialog for setting up display settings.
// Either triggered automatically by config data, or 
// triggered whenever an invalid display mode is set. 
//----------------------------------------------------------//


//-- This linker command forces the linker to use the v6 win32 controls
//-- (XP-style) rather than the older v5 (win95-win2k).
#pragma comment(linker,"\"/manifestdependency:type='win32'	\
name='Microsoft.Windows.Common-Controls' version='6.0.0.0'	\
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#include "resource.h"

#include "../SystemIncludes.h"
#include "../../../DS3D/Win32/Game.h"

#include "WinSysIncludes.h"


//----------------------------------------------------------//
// DEFINES
//----------------------------------------------------------//


#define WINSYS_CONFIG_SAVE_FILE			"../../Data/config.ini"


//----------------------------------------------------------//
// GLOBALS
//----------------------------------------------------------//


CWinSysConfigDialog::DesiredWindowMode CWinSysConfigDialog::sm_WindowModeTable[CWinSysConfigDialog::DesiredWindowMode::MAX] = {
	IDS_MODE_WINDOWED,		
	IDS_MODE_BORDERLESS,		
	IDS_MODE_FULLSCREEN		
};


CWinSysConfigDialog::DesiredResolution CWinSysConfigDialog::sm_ResolutionTable[CWinSysConfigDialog::DesiredResolution::MAX] = {
	{IDS_RES_640x480,		640,	480 },
	{IDS_RES_800x600,		800,	600 },
	{IDS_RES_1024x768,		1024,	768 },
	{IDS_RES_1280x720,		1280,	720 },
	{IDS_RES_1280x800,		1280,	800 },
	{IDS_RES_1366x768,		1366,	768 },
	{IDS_RES_1440x900,		1440,	900 },
	{IDS_RES_1600x900,		1600,	900 },
	{IDS_RES_1680x1050,		1680,	1050 },
	{IDS_RES_1920x1080,		1920,	1080 }
};


CWinSysConfigDialog::DesiredBPP CWinSysConfigDialog::sm_BPPTable[CWinSysConfigDialog::DesiredBPP::MAX] = {
	{ IDS_BPP_16,			16 },
	{ IDS_BPP_32,			32 }
};


//----------------------------------------------------------//
// CWinSysConfigDialog::CWinSysConfigDialog()
//----------------------------------------------------------//
//-- Description
// Constructor for a class that keeps track of the config
// dialog initialisation state and child rollup container.
//----------------------------------------------------------//
CWinSysConfigDialog::CWinSysConfigDialog()
{
	m_bRegistered = false;
}


//----------------------------------------------------------//
// CWinSysConfigDialog::~CWinSysConfigDialog()
//----------------------------------------------------------//
//-- Description
// Destructor for a class that keeps track of the config
// dialog initialisation state and child rollup container.
//----------------------------------------------------------//
CWinSysConfigDialog::~CWinSysConfigDialog()
{
	Shutdown();
}


//----------------------------------------------------------//
// CWinSysConfigDialog::EventProc
//----------------------------------------------------------//
//-- Description			
// Static event handler for the config dialog.
//----------------------------------------------------------//
INT_PTR CALLBACK CWinSysConfigDialog::StaticDlgEventProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	CWinSysConfigDialog* pThis = NULL;

	if (WM_INITDIALOG == nMsg)
	{
		pThis = reinterpret_cast<CWinSysConfigDialog*>(lParam);
		SetWindowLongPtr(hWnd, DWLP_USER, reinterpret_cast<LONG_PTR>(pThis));
	}
	else
	{
		pThis = reinterpret_cast<CWinSysConfigDialog*>(GetWindowLongPtr(hWnd, DWLP_USER));
	}

	if (IS_PTR(pThis))
	{
		return pThis->DlgEventProc(hWnd, nMsg, wParam, lParam);
	}

	return false;
}

INT_PTR CWinSysConfigDialog::DlgEventProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	switch (nMsg)
	{
		case WM_INITDIALOG:
		{
			m_RollupContainer.Construct(hWnd, IDC_CUSTOM_ROLLUPCONTAINER);

			if (IS_TRUE(ShowAtStartup()))
			{
				CheckDlgButton(hWnd, IDC_CHECK_SHOWATSTARTUP, BST_CHECKED);
			}
			else
			{
				CheckDlgButton(hWnd, IDC_CHECK_SHOWATSTARTUP, BST_UNCHECKED);
			}

			SetDlgItemText(hWnd, IDC_NEVISTA_VERSION, TEXT(Game_Version()));

			return true;
		}
		break;

		case WM_COMMAND:
		{
			switch (LOWORD(wParam))
			{
				case IDOK:
				{
					//-- Run
					EndDialog(hWnd, IDOK);

					return true;
				}
				break;

				case IDCANCEL:
				{
					//-- Don't run
					EndDialog(hWnd, IDCANCEL);

					return true;
				}
				break;

				case IDC_CHECK_SHOWATSTARTUP:
				{
//					if (m_ConfigIni.ReadInt("Config", "ShowDialogOnStartup") == 1)
					{
						CheckDlgButton(hWnd, IDC_CHECK_SHOWATSTARTUP, BST_UNCHECKED);
//						m_ConfigIni.WriteInt("Config", "ShowDialogOnStartup", 0);
					}
//					else
//					{
//						CheckDlgButton(hWnd, IDC_SHOWDIALOG, BST_CHECKED);
//						m_ConfigIni.WriteInt("Config", "ShowDialogOnStartup", 1);
//					}

					return true;
				}
				break;
				
				default:
				break;
			}
		}
		break;

		default:
		break;
	};
	
	return false;
}


//----------------------------------------------------------//
// CWinSysConfigDialog::Initialise
//----------------------------------------------------------//
//-- Description
// Initialises widgets and registers the rollup container
// class. Activates config dialog.
//----------------------------------------------------------//
s32 CWinSysConfigDialog::Initialise(void)
{
	s32 nReturnStatus = WINSYS_OK;

//	gDbgLog.Printf(">CWinSysConfigDialog::Initialise");

	if (IS_FALSE(m_bRegistered))
	{
		CWinSysRollupContainer::RegisterRollupContainerClass();

		m_bRegistered = true;
	}

	//TODO - init config ini

	nReturnStatus = m_RollupContainer.Initialise();
	if (WINSYS_OK != nReturnStatus)
	{
		//-- Failed to Initialise rollup container
		return nReturnStatus;
	}

	//-- Success
	return WINSYS_OK;
}


//----------------------------------------------------------//
// CWinSysConfigDialog::Shutdown
//----------------------------------------------------------//
//-- Description
// Cleans up any outstanding data for the config dialog.
//----------------------------------------------------------//
s32 CWinSysConfigDialog::Shutdown(void)
{
	m_RollupContainer.Shutdown();
	
//	m_ConfigIni.Save(WINSYS_CONFIG_SAVE_FILE);

	if (IS_TRUE(m_bRegistered))
	{
		CWinSysRollupContainer::UnregisterRollupContainerClass();
	}
	m_bRegistered = false;

	return WINSYS_OK;
}


//----------------------------------------------------------//
// CWinSysConfigDialog::Show
//----------------------------------------------------------//
//-- Description			
// Display the config dialog. The dialog is modal and will
// block until dismissed.
//----------------------------------------------------------//
s32 CWinSysConfigDialog::Show(void)
{
	INT_PTR ret = DialogBoxParam(GetModuleHandle(0), 
					MAKEINTRESOURCE(IDD_NEVISTA_CONFIG), 
					NULL, 
					CWinSysConfigDialog::StaticDlgEventProc,
					reinterpret_cast<LPARAM>(this));
	if (IDOK != ret)
	{
		//-- Failed to create config dialog
//		gDbgLog.Printf("<Could not create Config Dialog or Dialog \"Exit\" pressed.");

		return WINSYS_CONFIG_DIALOG_FAIL;
	}

//	gDbgLog.Printf("<Completed. (OK)");

	//-- Success
	return WINSYS_OK;
}


//----------------------------------------------------------//
// CWinSysConfigDialog::ShowAtStartup
//----------------------------------------------------------//
//-- Description	
// Query whether config dialog should be display at startup
//----------------------------------------------------------//
bool CWinSysConfigDialog::ShowAtStartup(void) const
{
//	if (IS_TRUE(m_ConfigIni.ReadInt("Config", "ShowDialogOnStartup")))
//	{
		return true;
//	}
//
//	return false;
}


//----------------------------------------------------------//
// CWinSysConfigDialog::GetDesiredWidth
//----------------------------------------------------------//
//-- Description
// Returns the desired config width
//----------------------------------------------------------//
s32 CWinSysConfigDialog::GetDesiredWidth(void) const
{
	return sm_ResolutionTable[DesiredResolution::Res640x480].m_nWidth; //m_ConfigIni.ReadInt("Window", "Width");
}


//----------------------------------------------------------//
// CWinSysConfigDialog::GetDesiredHeight
//----------------------------------------------------------//
//-- Description
// Returns the desired config height
//----------------------------------------------------------//
s32 CWinSysConfigDialog::GetDesiredHeight(void) const
{
	return sm_ResolutionTable[DesiredResolution::Res640x480].m_nHeight; //m_ConfigIni.ReadInt("Window", "Height");
}


//----------------------------------------------------------//
// CWinSysConfigDialog::GetDesiredBPP
//----------------------------------------------------------//
//-- Description
// Returns the desired config BPP
//----------------------------------------------------------//
s32 CWinSysConfigDialog::GetDesiredBPP(void) const
{
	return sm_BPPTable[DesiredBPP::Bpp16].m_nBPP; //m_ConfigIni.ReadInt("Window", "BPP");
}


//----------------------------------------------------------//
// CWinSysConfigDialog::GetDesiredWindowMode
//----------------------------------------------------------//
//-- Description
// Returns the desired config window mode
//----------------------------------------------------------//
CWinSysConfigDialog::DesiredWindowMode::Enum CWinSysConfigDialog::GetDesiredWindowMode(void) const
{
/*	switch (m_ConfigIni.ReadInt("Window", "Mode"))
	{
		case DesiredWindowMode::Fullscreen:
		{
			return DesiredWindowMode::Fullscreen;
		}
		break;
		case DesiredWindowMode::Borderless:
		{
			return DesiredWindowMode::Borderless;
		}
		break;
		default:
		break;
	}
*/
	return DesiredWindowMode::Windowed;
}



//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//
