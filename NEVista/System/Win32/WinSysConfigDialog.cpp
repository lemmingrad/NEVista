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


#define WINSYS_CONFIG_SAVE_FILE					"Data/config.ini"

#define WINSYS_CONFIG_SECTION_CONFIG			"Config"
#define WINSYS_CONFIG_SHOWATSTARTUP				"ShowDialogOnStartup"
#define WINSYS_CONFIG_SECTION_WINDOW			"Window"
#define WINSYS_CONFIG_WINDOWMODE				"Mode"
#define WINSYS_CONFIG_RESOLUTION				"Resolution"
#define WINSYS_CONFIG_BPP						"BPP"
#define WINSYS_CONFIG_SECTION_NETWORK			"Network"
#define WINSYS_CONFIG_SERVER					"Server"


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
					ChangeShowAtStartup(BST_CHECKED == IsDlgButtonChecked(hWnd, IDC_CHECK_SHOWATSTARTUP));
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

	gDebugLog.Printf("CWinSysConfigDialog::Initialise:");
	SCOPED_LOG_INDENT(gDebugLog);

	if (IS_FALSE(m_bRegistered))
	{
		CWinSysRollupContainer::RegisterRollupContainerClass();

		m_bRegistered = true;
	}

	//TODO - init config ini
	if (IS_FALSE(m_ConfigIni.Initialise()))
	{
		// Failed to initialise config ini
		return WINSYS_CONFIG_INI_NOT_LOADED;
	}

	ChangeShowAtStartup(true);
	SetDesiredWindowMode(DesiredWindowMode::Windowed);
	SetDesiredResolution(DesiredResolution::Res640x480);
	SetDesiredBPP(DesiredBPP::Bpp32);
	SetServerAddress("192.168.1.100:1234");

	if (IS_FALSE(m_ConfigIni.Load(WINSYS_CONFIG_SAVE_FILE)))
	{
		// Failed to load config ini
		MessageBox(NULL, "Configuration Ini cannot be created. Using defaults.", Game_Title(), MB_OK|MB_ICONEXCLAMATION);
	}

	nReturnStatus = m_RollupContainer.Initialise(this);
	if (WINSYS_OK != nReturnStatus)
	{
		//-- Failed to Initialise rollup container
		gDebugLog.Printf("Failed to initialise rollup container.");
		return nReturnStatus;
	}

	gDebugLog.Printf("Complete. (OK)");

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
	
	m_ConfigIni.Save(WINSYS_CONFIG_SAVE_FILE);
	m_ConfigIni.Shutdown();

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
	gDebugLog.Printf("Show Config Dialog:");
	SCOPED_LOG_INDENT(gDebugLog);

	INT_PTR ret = DialogBoxParam(GetModuleHandle(0), 
					MAKEINTRESOURCE(IDD_NEVISTA_CONFIG), 
					NULL, 
					CWinSysConfigDialog::StaticDlgEventProc,
					reinterpret_cast<LPARAM>(this));
	if (IDOK != ret)
	{
		//-- Failed to create config dialog
		gDebugLog.Printf("Could not create Config Dialog or Dialog \"Exit\" pressed.");

		return WINSYS_CONFIG_DIALOG_FAIL;
	}

	gDebugLog.Printf("Completed. (OK)");

	//-- Success
	return WINSYS_OK;
}


//----------------------------------------------------------//
// CWinSysConfigDialog::ShowAtStartup
//----------------------------------------------------------//
//-- Description	
// Query whether config dialog should be display at startup
//----------------------------------------------------------//
bool CWinSysConfigDialog::ShowAtStartup(void)
{
	if (m_ConfigIni.ReadInt(WINSYS_CONFIG_SECTION_CONFIG, WINSYS_CONFIG_SHOWATSTARTUP) == 1)
	{
		return true;
	}

	return false;
}


//----------------------------------------------------------//
// CWinSysConfigDialog::ChangeShowAtStartup
//----------------------------------------------------------//
//-- Description	
// Change 'show on startup' setting
//----------------------------------------------------------//
void CWinSysConfigDialog::ChangeShowAtStartup(bool bShow)
{
	m_ConfigIni.WriteInt(WINSYS_CONFIG_SECTION_CONFIG, WINSYS_CONFIG_SHOWATSTARTUP, IS_TRUE(bShow));
}


//----------------------------------------------------------//
// CWinSysConfigDialog::GetDesiredResolution
//----------------------------------------------------------//
//-- Description
// Returns the desired config resolution enum
//----------------------------------------------------------//
CWinSysConfigDialog::DesiredResolution::Enum CWinSysConfigDialog::GetDesiredResolution(void)
{
	s32 nRes = m_ConfigIni.ReadInt(WINSYS_CONFIG_SECTION_WINDOW, WINSYS_CONFIG_RESOLUTION);
	if (nRes < DesiredResolution::MAX)
	{
		return (DesiredResolution::Enum)nRes;
	}

	return DesiredResolution::Res640x480;
}


//----------------------------------------------------------//
// CWinSysConfigDialog::SetDesiredResolution
//----------------------------------------------------------//
//-- Description
// Set the desired config resolution enum
//----------------------------------------------------------//
void CWinSysConfigDialog::SetDesiredResolution(CWinSysConfigDialog::DesiredResolution::Enum eRes)
{
	if (eRes < DesiredResolution::MAX)
	{
		m_ConfigIni.WriteInt(WINSYS_CONFIG_SECTION_WINDOW, WINSYS_CONFIG_RESOLUTION, eRes);
	}
	else
	{
		m_ConfigIni.WriteInt(WINSYS_CONFIG_SECTION_WINDOW, WINSYS_CONFIG_RESOLUTION, DesiredResolution::Res640x480);
	}
}


//----------------------------------------------------------//
// CWinSysConfigDialog::GetDesiredResolutionWidth
//----------------------------------------------------------//
//-- Description
// Returns the desired config width
//----------------------------------------------------------//
s32 CWinSysConfigDialog::GetDesiredResolutionWidth(void)
{
	return sm_ResolutionTable[GetDesiredResolution()].m_nWidth;
}


//----------------------------------------------------------//
// CWinSysConfigDialog::GetDesiredResolutionHeight
//----------------------------------------------------------//
//-- Description
// Returns the desired config height
//----------------------------------------------------------//
s32 CWinSysConfigDialog::GetDesiredResolutionHeight(void)
{
	return sm_ResolutionTable[GetDesiredResolution()].m_nHeight;
}


//----------------------------------------------------------//
// CWinSysConfigDialog::GetDesiredBPP
//----------------------------------------------------------//
//-- Description
// Returns the desired config BPP
//----------------------------------------------------------//
CWinSysConfigDialog::DesiredBPP::Enum CWinSysConfigDialog::GetDesiredBPP(void)
{
	s32 nBPP = m_ConfigIni.ReadInt(WINSYS_CONFIG_SECTION_WINDOW, WINSYS_CONFIG_BPP);
	if (nBPP < DesiredBPP::MAX)
	{
		return (DesiredBPP::Enum)nBPP;
	}

	return DesiredBPP::Bpp16;
}


//----------------------------------------------------------//
// CWinSysConfigDialog::SetDesiredBPP
//----------------------------------------------------------//
//-- Description
// Set the desired config BPP enum
//----------------------------------------------------------//
void CWinSysConfigDialog::SetDesiredBPP(CWinSysConfigDialog::DesiredBPP::Enum eBPP)
{
	if (eBPP < DesiredBPP::MAX)
	{
		m_ConfigIni.WriteInt(WINSYS_CONFIG_SECTION_WINDOW, WINSYS_CONFIG_BPP, eBPP);
	}
	else
	{
		m_ConfigIni.WriteInt(WINSYS_CONFIG_SECTION_WINDOW, WINSYS_CONFIG_BPP, DesiredBPP::Bpp16);
	}
}


//----------------------------------------------------------//
// CWinSysConfigDialog::GetDesiredBPPValue
//----------------------------------------------------------//
//-- Description
// Returns the desired config BPP value
//----------------------------------------------------------//
s32 CWinSysConfigDialog::GetDesiredBPPValue(void)
{
	return sm_BPPTable[GetDesiredBPP()].m_nBPP;
}


//----------------------------------------------------------//
// CWinSysConfigDialog::GetDesiredWindowMode
//----------------------------------------------------------//
//-- Description
// Returns the desired config window mode
//----------------------------------------------------------//
CWinSysConfigDialog::DesiredWindowMode::Enum CWinSysConfigDialog::GetDesiredWindowMode(void)
{
	s32 nWM = m_ConfigIni.ReadInt(WINSYS_CONFIG_SECTION_WINDOW, WINSYS_CONFIG_WINDOWMODE);
	if (nWM < DesiredWindowMode::MAX)
	{
		return (DesiredWindowMode::Enum)nWM;
	}

	return DesiredWindowMode::Windowed;
}


//----------------------------------------------------------//
// CWinSysConfigDialog::GetDesiredWindowMode
//----------------------------------------------------------//
//-- Description
// Returns the desired config window mode
//----------------------------------------------------------//
void CWinSysConfigDialog::SetDesiredWindowMode(CWinSysConfigDialog::DesiredWindowMode::Enum eWM)
{
	if (eWM < DesiredWindowMode::MAX)
	{
		m_ConfigIni.WriteInt(WINSYS_CONFIG_SECTION_WINDOW, WINSYS_CONFIG_WINDOWMODE, eWM);
	}
	else
	{
		m_ConfigIni.WriteInt(WINSYS_CONFIG_SECTION_WINDOW, WINSYS_CONFIG_WINDOWMODE, DesiredWindowMode::Windowed);
	}
}


//----------------------------------------------------------//
// CWinSysConfigDialog::GetServerAddress
//----------------------------------------------------------//
//-- Description
// Returns the desired config server address
//----------------------------------------------------------//
const s8* CWinSysConfigDialog::GetServerAddress(void)
{
	return m_ConfigIni.ReadString(WINSYS_CONFIG_SECTION_NETWORK, WINSYS_CONFIG_SERVER);
}


//----------------------------------------------------------//
// CWinSysConfigDialog::SetServerAddress
//----------------------------------------------------------//
//-- Description
// Sets the desired config server address
//----------------------------------------------------------//
void CWinSysConfigDialog::SetServerAddress(const s8* strAddress)
{
	m_ConfigIni.WriteString(WINSYS_CONFIG_SECTION_NETWORK, WINSYS_CONFIG_SERVER, strAddress);
}


//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//
