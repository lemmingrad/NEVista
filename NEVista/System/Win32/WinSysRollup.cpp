//----------------------------------------------------------//
// WINSYSROLLUP.CPP
//----------------------------------------------------------//
//-- Description			
// Class for a rollup control made from a windows dialog.
// Rollups can be added to a rollup container and displayed
// inside another dialog.
//----------------------------------------------------------//

#include "resource.h"
#include "WinSysIncludes.h"
#include "Types.h"


//----------------------------------------------------------//
// DEFINES
//----------------------------------------------------------//


#define ROLLUP_HEADER_HEIGHT (20)
#define ROLLUP_EXPANDER_SPACING (3)

//----------------------------------------------------------//
// GLOBALS
//----------------------------------------------------------//

//----------------------------------------------------------//
// CWinSysRollup::CWinSysRollup
//----------------------------------------------------------//
//-- Description
// Constructor for a rollup object.
//----------------------------------------------------------//
CWinSysRollup::CWinSysRollup(CWinSysRollupContainer* pContainer, const s8* strName, s32 nDialogID, State::Enum eInitialState)
{
	m_strName.Set(strName);

	m_nDialogID = nDialogID;
	m_hRollupWnd = NULL;

	m_nMaxHeight = 0;
	m_nAnimatedHeight = 0;

	m_pParent = pContainer;

	switch (eInitialState)
	{
		case State::FixedUnrolled:
		case State::Unrolled:
		case State::RolledUp:
		{
			m_eState = eInitialState;
		}
		break;

		default:
		{
			m_eState = State::Unrolled;
		}
		break;
	}	
}


//----------------------------------------------------------//
// CWinSysRollup::~CWinSysRollup
//----------------------------------------------------------//
//-- Description
// Destructor for a rollup object.
//----------------------------------------------------------//
CWinSysRollup::~CWinSysRollup()
{
	//-- For safety, destroy the dialog on destructor.
	//-- It should already be destroyed when parent container was destroyed.
	if (m_hRollupWnd)
	{
		DestroyWindow(m_hRollupWnd);
	}
	m_hRollupWnd = NULL;
}


//----------------------------------------------------------//
// CWinSysRollup::DlgEventProc
//----------------------------------------------------------//
//-- Description
// Window procedure	for handling events to rollups.
//----------------------------------------------------------//
INT_PTR CALLBACK CWinSysRollup::StaticDlgEventProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	CWinSysRollup* pThis = NULL;

	if (WM_INITDIALOG == nMsg)
	{
		pThis = reinterpret_cast<CWinSysRollup*>(lParam);
		SetWindowLongPtr(hWnd, DWLP_USER, reinterpret_cast<LONG_PTR>(pThis));
	}
	else
	{
		pThis = reinterpret_cast<CWinSysRollup*>(GetWindowLongPtr(hWnd, DWLP_USER));
	}

	if (IS_PTR(pThis))
	{
		return pThis->DlgEventProc(hWnd, nMsg, wParam, lParam);
	}

	return false;
}

INT_PTR CWinSysRollup::DlgEventProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	switch (nMsg)
	{
		case WM_INITDIALOG:
		{
			FixedString<256> strTempReadBuffer;

			CWinSysConfigDialog::DesiredWindowMode::Enum eWM = GetParentConfigDialog()->GetDesiredWindowMode();
			HWND hComboWM = GetDlgItem(hWnd, IDC_COMBO_WINDOWMODE);
			if (hComboWM)
			{
				for (s32 i = 0; i < CWinSysConfigDialog::DesiredWindowMode::MAX; ++i)
				{
					LoadString(GetModuleHandle(0), CWinSysConfigDialog::sm_WindowModeTable[i].m_nStringID, strTempReadBuffer.Buffer(), strTempReadBuffer.Size());
					ComboBox_AddString(hComboWM, TEXT(strTempReadBuffer.ConstBuffer()));
				}

				ComboBox_SetCurSel(hComboWM, eWM);
			}

			CWinSysConfigDialog::DesiredResolution::Enum eResolution = GetParentConfigDialog()->GetDesiredResolution();
			HWND hComboResolution = GetDlgItem(hWnd, IDC_COMBO_RESOLUTION);
			if (hComboResolution)
			{
				for (s32 i = 0; i < CWinSysConfigDialog::DesiredResolution::MAX; ++i)
				{
					LoadString(GetModuleHandle(0), CWinSysConfigDialog::sm_ResolutionTable[i].m_nStringID, strTempReadBuffer.Buffer(), strTempReadBuffer.Size());
					ComboBox_AddString(hComboResolution, TEXT(strTempReadBuffer.ConstBuffer()));
				}

				ComboBox_SetCurSel(hComboResolution, eResolution);

				EnableWindow(hComboResolution, (CWinSysConfigDialog::DesiredWindowMode::Borderless != eWM));
			}

			CWinSysConfigDialog::DesiredBPP::Enum eBPP = GetParentConfigDialog()->GetDesiredBPP();
			HWND hComboBPP = GetDlgItem(hWnd, IDC_COMBO_BPP);
			if (hComboBPP)
			{
				for (s32 i = 0; i < CWinSysConfigDialog::DesiredBPP::MAX; ++i)
				{
					LoadString(GetModuleHandle(0), CWinSysConfigDialog::sm_BPPTable[i].m_nStringID, strTempReadBuffer.Buffer(), strTempReadBuffer.Size());
					ComboBox_AddString(hComboBPP, TEXT(strTempReadBuffer.ConstBuffer()));
				}

				ComboBox_SetCurSel(hComboBPP, eBPP);
			}

			const s8* strServerAddress = GetParentConfigDialog()->GetServerAddress();
			HWND hServerAddress = GetDlgItem(hWnd, IDC_EDIT_SERVER_ADDRESS);
			Edit_SetText(hServerAddress, TEXT(strServerAddress));		

			const s8* strServerEmail = GetParentConfigDialog()->GetServerLoginEmail();
			HWND hServerEmail = GetDlgItem(hWnd, IDC_EDIT_LOGIN_EMAIL);
			Edit_SetText(hServerEmail, TEXT(strServerEmail));		

			const s8* strServerPass = GetParentConfigDialog()->GetServerLoginPassword();
			HWND hServerPass = GetDlgItem(hWnd, IDC_EDIT_LOGIN_PASSWORD);
			Edit_SetText(hServerPass, TEXT(strServerPass));		

			bool bValidScreenMode = true; 
			if (IS_TRUE(bValidScreenMode))
			{
				LoadString(GetModuleHandle(0), IDS_CONFIG_WELCOME, strTempReadBuffer.Buffer(), strTempReadBuffer.Size());
			}
			else
			{
				LoadString(GetModuleHandle(0), IDS_CONFIG_ERROR, strTempReadBuffer.Buffer(), strTempReadBuffer.Size());
			}
			SetDlgItemText(hWnd, IDC_MESSAGES, strTempReadBuffer.ConstBuffer());

			return true;
		}
		break;

		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_MOUSEMOVE:
		{
			m_pParent->CancelScroll();
			return true;
		}
		break;

		case WM_COMMAND:
		{
			switch(LOWORD(wParam))
			{
				case IDC_COMBO_WINDOWMODE:
				{
					if (CBN_SELCHANGE == HIWORD(wParam))
					{
						s32 nWM = ComboBox_GetCurSel((HWND)lParam); 
						CWinSysConfigDialog::DesiredWindowMode::Enum eWM = (CWinSysConfigDialog::DesiredWindowMode::Enum)nWM;
					
						GetParentConfigDialog()->SetDesiredWindowMode(eWM);

						EnableWindow(GetDlgItem(hWnd, IDC_COMBO_RESOLUTION), (CWinSysConfigDialog::DesiredWindowMode::Borderless != eWM));

						return true;
					}
				}
				break;

				case IDC_COMBO_RESOLUTION:
				{
					if (CBN_SELCHANGE == HIWORD(wParam))
					{
						s32 nRes = ComboBox_GetCurSel((HWND)lParam); 
						GetParentConfigDialog()->SetDesiredResolution((CWinSysConfigDialog::DesiredResolution::Enum)nRes);
						return true;
					}
				}
				break;

				case IDC_COMBO_BPP:
				{
					if (CBN_SELCHANGE == HIWORD(wParam))
					{
						s32 nBPP = ComboBox_GetCurSel((HWND)lParam); 
						GetParentConfigDialog()->SetDesiredBPP((CWinSysConfigDialog::DesiredBPP::Enum)nBPP);
						return true;
					}
				}
				break;

				case IDC_EDIT_SERVER_ADDRESS:
				{
					FixedString<256> strServerAddress;

					if (EN_CHANGE == HIWORD(wParam))
					{
						if (Edit_GetText((HWND)lParam, strServerAddress.Buffer(), strServerAddress.Size()) > 0)
						{
							GetParentConfigDialog()->SetServerAddress(strServerAddress.ConstBuffer());
						}
						return true;
					}
				}
				break;

				case IDC_EDIT_LOGIN_EMAIL:
				{
					FixedString<256> strServerEmail;

					if (EN_CHANGE == HIWORD(wParam))
					{
						if (Edit_GetText((HWND)lParam, strServerEmail.Buffer(), strServerEmail.Size()) > 0)
						{
							GetParentConfigDialog()->SetServerLoginEmail(strServerEmail.ConstBuffer());
						}
						return true;
					}
				}
				break;

				case IDC_EDIT_LOGIN_PASSWORD:
				{
					FixedString<256> strServerPass;

					if (EN_CHANGE == HIWORD(wParam))
					{
						if (Edit_GetText((HWND)lParam, strServerPass.Buffer(), strServerPass.Size()) > 0)
						{
							GetParentConfigDialog()->SetServerLoginPassword(strServerPass.ConstBuffer());
						}
						return true;
					}
				}
				break;
			}
		}
		break;

		default:
		break;
	}

	return false;
}


//----------------------------------------------------------//
// CWinSysRollup::Create
//----------------------------------------------------------//
//-- Description
// Builds the rollup from available dialog resources, defines 
// a bounding box and positions the rollup correctly.
//----------------------------------------------------------//
s32 CWinSysRollup::Construct(HWND hParentContainer)
{
	m_hRollupWnd = CreateDialogParam(GetModuleHandle(NULL), 
							MAKEINTRESOURCE(m_nDialogID),  
							hParentContainer, 
							CWinSysRollup::StaticDlgEventProc,
							reinterpret_cast<LPARAM>(this));

	if (IS_ZERO(m_hRollupWnd))
	{
		return WINSYS_CONFIG_ROLLUPCONTAINER_ADDROLLUP_FAIL;
	}

	ShowWindow(m_hRollupWnd, SW_SHOW);

	RECT WindowRect;
	GetWindowRect(m_hRollupWnd, &WindowRect);
	m_nMaxHeight = WindowRect.bottom - WindowRect.top;

	m_RollupArea.left = ROLLUPCONTAINER_SPACING;
	m_RollupArea.right = m_pParent->GetContainerArea().right - ROLLUPCONTAINER_SCROLLBAR_SPACING;
	m_RollupArea.top = 0;
	m_RollupArea.bottom = 0;

	m_ExpanderArrowArea.top = m_RollupArea.top + ROLLUP_EXPANDER_SPACING;
	m_ExpanderArrowArea.left = m_RollupArea.right - ROLLUP_HEADER_HEIGHT + ROLLUP_EXPANDER_SPACING;
	m_ExpanderArrowArea.bottom = m_RollupArea.top + ROLLUP_HEADER_HEIGHT - ROLLUP_EXPANDER_SPACING;
	m_ExpanderArrowArea.right = m_RollupArea.right - ROLLUP_EXPANDER_SPACING;

	return WINSYS_OK;
}


//----------------------------------------------------------//
// CWinSysRollup::ProcessInput
//----------------------------------------------------------//
//-- Description
// Handles mouse input within the rollup area. 
// Note that this does not handle clicks on items within the
// rollup; that is handled automatically and passed to the 
// WinSys_RollupProc function as a WM_COMMAND event.
//----------------------------------------------------------//
s32 CWinSysRollup::ProcessInput(UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	s32 nMouseX = GET_X_LPARAM(lParam);
	s32 nMouseY = GET_Y_LPARAM(lParam);

	//-- Only process left mouse button clicks
	if (WM_LBUTTONDOWN != nMsg)
	{
		return WINSYS_CONFIG_INPUT_NOT_PROCESSED;
	}
		
	if (IS_TRUE(IsInsideArea(nMouseX, nMouseY, m_ExpanderArrowArea)))
	{
		switch (m_eState)
		{
			case State::Unrolling:
			case State::Unrolled:
			{
				m_eState = State::RollingUp;
			}
			break;

			case State::RollingUp:
			case State::RolledUp:
			{
				m_eState = State::Unrolling;
			}
			break;

			default:
			break;
		}

		return WINSYS_OK;
	}

	return WINSYS_CONFIG_INPUT_NOT_PROCESSED;
}


//----------------------------------------------------------//
// CWinSysRollup::Update
//----------------------------------------------------------//
//-- Description
// Update function for the rollup, mostly used for animation
// but also determines whether the animation state requires
// a redraw of the rollup.
//----------------------------------------------------------//
bool CWinSysRollup::Update(void)
{
	//-- Perform animation of rollup
	switch (m_eState)
	{
		case State::Unrolled:
		case State::FixedUnrolled:
		default:
		{
			m_nAnimatedHeight = m_nMaxHeight;
		}
		break;

		case State::RolledUp:
		{
			m_nAnimatedHeight = 0;
		}
		break;

		case State::RollingUp:
		{
			f32 fCurrent = (f32)m_nAnimatedHeight;
			f32 fMovement = fCurrent;

			fMovement *= 0.5f;

			fCurrent -= fMovement;

			if (fCurrent < 1.0f)
			{
				//-- close enough
				m_eState = State::RolledUp;
				m_nAnimatedHeight = 0;
			}
			else
			{
				m_nAnimatedHeight = (s32)fCurrent;
			}

			return true;
		}
		break;

		case State::Unrolling:
		{
			f32 fTarget = (f32)m_nMaxHeight;
			f32 fCurrent = (f32)m_nAnimatedHeight;
			f32 fMovement = fTarget - fCurrent;

			fMovement *= 0.5f;

			fCurrent += fMovement;

			if ((fTarget - fCurrent) < 1.0f)
			{
				//-- close enough
				m_eState = State::Unrolled;
				m_nAnimatedHeight = m_nMaxHeight;
			}
			else
			{
				m_nAnimatedHeight = (s32)fCurrent;
			}

			return true;
		}
		break;
	}

	return false;
}


//----------------------------------------------------------//
// CWinSysRollup::Reposition
//----------------------------------------------------------//
//-- Description
// Positions the rollup correctly within the parent container.
//----------------------------------------------------------//
void CWinSysRollup::Reposition(s32* pnOffset)
{
	m_RollupArea.top = *pnOffset;
	m_RollupArea.bottom = m_RollupArea.top + ROLLUP_HEADER_HEIGHT + m_nAnimatedHeight;

	m_ExpanderArrowArea.top = m_RollupArea.top + ROLLUP_EXPANDER_SPACING;
	m_ExpanderArrowArea.bottom = m_RollupArea.top + ROLLUP_HEADER_HEIGHT - ROLLUP_EXPANDER_SPACING;

	MoveWindow(m_hRollupWnd, 
				m_RollupArea.left, 
				m_RollupArea.top + ROLLUP_HEADER_HEIGHT, 
				m_RollupArea.right - m_RollupArea.left,
				m_nAnimatedHeight,
				false);

	*pnOffset += ROLLUP_HEADER_HEIGHT;
	*pnOffset += m_nAnimatedHeight;
	*pnOffset += ROLLUPCONTAINER_SPACING;
}


//----------------------------------------------------------//
// CWinSysRollup::Render
//----------------------------------------------------------//
//-- Description
// Redraws the rollup client area.
// Note this does not draw the dialog resource within the 
// rollup, it only draws the title bar at the top.
//----------------------------------------------------------//
s32 CWinSysRollup::Render(HDC hDC)
{
	POINT point[3];

	COLORREF nOldTextColour		= SetTextColor(hDC, 0x000000);
	s32		 nOldBackgroundMode	= SetBkMode(hDC, TRANSPARENT);
	HFONT	 hOldFont			= SelectFont(hDC, GetStockObject(DEFAULT_GUI_FONT));
	HPEN	 hOldPen			= (HPEN)SelectObject(hDC, GetStockObject(BLACK_PEN));
	
	//-- Draw header background - large grey square
	RECT rectHeaderBar;
	rectHeaderBar.left = m_RollupArea.left;
	rectHeaderBar.right = m_RollupArea.right;
	rectHeaderBar.top = m_RollupArea.top;
	rectHeaderBar.bottom = m_RollupArea.top + ROLLUP_HEADER_HEIGHT;

	FillRect(hDC, &rectHeaderBar, (HBRUSH)GetStockObject(GRAY_BRUSH));

	//-- Draw header underline, dark grey/black line
	RECT rectHeaderShadow = rectHeaderBar;
	rectHeaderShadow.bottom = rectHeaderBar.top + ROLLUPCONTAINER_SPACING;
	
	FillRect(hDC, &rectHeaderShadow, (HBRUSH)GetStockObject(BLACK_BRUSH));

	//-- Draw text, black
	RECT rectHeaderText = rectHeaderBar;
	rectHeaderText.left += ROLLUPCONTAINER_SPACING;
	rectHeaderText.right = m_ExpanderArrowArea.left - ROLLUPCONTAINER_SPACING;
	rectHeaderText.top = rectHeaderShadow.bottom + ROLLUPCONTAINER_SPACING;
	rectHeaderText.bottom -= ROLLUPCONTAINER_SPACING;

	DrawText(hDC, m_strName.ConstBuffer(), m_strName.Length(), &rectHeaderText, DT_LEFT);

	//-- Draw min/max button outline, white highlight, dark gray/black shadow
	switch (m_eState)
	{
		case State::RolledUp:
		case State::RollingUp:
		{
			point[0].x = m_ExpanderArrowArea.right;	
			point[0].y = m_ExpanderArrowArea.top;
			point[1].x = m_ExpanderArrowArea.right;
			point[1].y = m_ExpanderArrowArea.bottom;
			point[2].x = m_ExpanderArrowArea.left;
			point[2].y = m_ExpanderArrowArea.bottom;

			Polyline(hDC, point, 3);

			point[1].x = point[2].x;

			SelectObject(hDC, GetStockObject(WHITE_PEN));
			Polyline(hDC, point, 2);
		}
		break;

		case State::Unrolled:
		case State::Unrolling:
		{
			point[0].x = m_ExpanderArrowArea.right;		
			point[0].y = m_ExpanderArrowArea.top;
			point[1].x = m_ExpanderArrowArea.left;
			point[1].y = m_ExpanderArrowArea.bottom;

			Polyline(hDC, point, 2);

			point[2] = point[1];
			point[1].y = point[0].y;

			SelectObject(hDC, GetStockObject(WHITE_PEN));
			Polyline(hDC, point, 3);
		}
		break;

		default:
		break;
	}

	//-- And restore previous
	SetBkMode(hDC, nOldBackgroundMode);
	SelectFont(hDC, hOldFont);
	SetTextColor(hDC, nOldTextColour);
	SelectObject(hDC, hOldPen);

	return WINSYS_OK;
}


//----------------------------------------------------------//
// CWinSysRollup::IsInsideArea
//----------------------------------------------------------//
//-- Description
// Test to see if a coordinate is inside a given rectangle.
//----------------------------------------------------------//
bool CWinSysRollup::IsInsideArea(s32 nMouseX, s32 nMouseY, RECT& area)
{
	if ( (nMouseX < area.left) 
		|| (nMouseX >= area.right)
		|| (nMouseY < area.top)
		|| (nMouseY >= area.bottom) )
	{
		return false;
	}

	return true;
}

CWinSysConfigDialog* CWinSysRollup::GetParentConfigDialog(void)
{
	return m_pParent->GetParentConfigDialog();
}



//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//
