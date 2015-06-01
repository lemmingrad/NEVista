//----------------------------------------------------------//
// WINSYSROLLUPCONTAINER.CPP
//----------------------------------------------------------//
//-- Description			
// Class for a rollup container object, made from a dummy
// window object inside a dialog. The container represents
// the area inside which child rollups exist.
//----------------------------------------------------------//

#include "resource.h"

#include "../SystemIncludes.h"

#include "WinSysIncludes.h"


//----------------------------------------------------------//
// DEFINES
//----------------------------------------------------------//


#define WINSYS_ROLLUP_CONTAINER_CLASS_NAME			"WinSysRollupContainer"


//----------------------------------------------------------//
// GLOBALS
//----------------------------------------------------------//


const u32 WINSYS_ROLLUP_CONTAINER_TIMER_ID			= 1;
const u32 WINSYS_ROLLUP_CONTAINER_REFRESH_TIME		= 100;	//-- 100ms refresh timer


//----------------------------------------------------------//
// CWinSysRollupContainer::CWinSysRollupContainer
//----------------------------------------------------------//
//-- Description
// Constructor for a rollup container object. The container
// is a dummy window object within the config dialog which is
// simply used to define a clipping region and position.
//----------------------------------------------------------//
CWinSysRollupContainer::CWinSysRollupContainer()
{
	m_hRollupContainerWnd = NULL;
}


//----------------------------------------------------------//
// CWinSysRollupContainer::~CWinSysRollupContainer
//----------------------------------------------------------//
//-- Description
// Destructor for a rollup container object.
//----------------------------------------------------------//
CWinSysRollupContainer::~CWinSysRollupContainer()
{
	Shutdown();
}


//----------------------------------------------------------//
// CWinSysRollupContainer::Initialise
//----------------------------------------------------------//
//-- Description
// Initialise rollup container.
//----------------------------------------------------------//
s32 CWinSysRollupContainer::Initialise(void)
{
	m_Rollups.clear();

	//-- Add rollups
	if (WINSYS_OK != AddRollup("Messages", IDD_ROLLUP_MESSAGES, CWinSysRollup::State::FixedUnrolled))
	{
		//-- Failed to add rollup
	}

	if (WINSYS_OK != AddRollup("Window", IDD_ROLLUP_WINDOW_SETTINGS, CWinSysRollup::State::Unrolled))
	{
		//-- Failed to add rollup
	}

	if (WINSYS_OK != AddRollup("Network", IDD_ROLLUP_NETWORK, CWinSysRollup::State::RolledUp))
	{
		//-- Failed to add rollup
	}

	if (WINSYS_OK != AddRollup("OpenGL", IDD_ROLLUP_GL_SETTINGS, CWinSysRollup::State::RolledUp))
	{
		//-- Failed to add rollup
	}

	m_hRollupContainerWnd = NULL;

	//-- Success
	return WINSYS_OK;
}


//----------------------------------------------------------//
// CWinSysRollupContainer::Shutdown
//----------------------------------------------------------//
//-- Description
// Shutdown container object
//----------------------------------------------------------//
s32 CWinSysRollupContainer::Shutdown(void)
{
	if (m_hRollupContainerWnd)
	{
		KillTimer(m_hRollupContainerWnd, WINSYS_ROLLUP_CONTAINER_TIMER_ID);
	}
	m_hRollupContainerWnd = NULL;

	m_Rollups.clear();

	return WINSYS_OK;
}


//----------------------------------------------------------//
// CWinSysRollupContainer::EventProc
//----------------------------------------------------------//
//-- Description
// Window Procedure for handling events sent to the rollup 
// container window.
// Container window will process any mouse clicks within
// title bar areas of the rollups, because the title bars are
// not part of the rollup dialog resources, and so will NOT 
// generate events in WinSys_RollupProc.
// The rollup container also holds the animation timer used
// to animate the rollups.
//----------------------------------------------------------//
LRESULT CALLBACK CWinSysRollupContainer::StaticWndEventProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	CWinSysRollupContainer* pThis = reinterpret_cast<CWinSysRollupContainer*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	if (pThis)
	{
		return pThis->WndEventProc(hWnd, nMsg, wParam, lParam);
	}

	return DefWindowProc(hWnd, nMsg, wParam, lParam);
}

LRESULT CWinSysRollupContainer::WndEventProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	switch (nMsg)
	{
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_MOUSEMOVE:
		{
			if (WINSYS_OK == ProcessInput(nMsg, wParam, lParam))
			{
				return 0;
			}
		}
		break;

		case WM_TIMER:
		{
			Update(false);
			return 0;
		}
		break;

		case WM_PAINT:
		{
			Render();
			return 0;
		}
		break;

		default:
		break;
	}
	
	return DefWindowProc(hWnd, nMsg, wParam, lParam);
} 


//----------------------------------------------------------//
// CWinSysRollupContainer::RegisterRollupContainerClass
//----------------------------------------------------------//
//-- Description
// Function to register a class for the dummy rollup container
// window that will appear on a dialog.
// NOTE this MUST be called before creating the dialog the 
// rollup container exists on, otherwise the rollup container
// will not process events and messages.
//----------------------------------------------------------//
s32 CWinSysRollupContainer::RegisterRollupContainerClass(void)
{
	WNDCLASSEX RollupContainerClass;
	
	RollupContainerClass.cbSize			= sizeof(RollupContainerClass);
	RollupContainerClass.lpszClassName	= TEXT(WINSYS_ROLLUP_CONTAINER_CLASS_NAME);
	RollupContainerClass.hInstance		= GetModuleHandle(0);
	RollupContainerClass.lpfnWndProc	= (WNDPROC)CWinSysRollupContainer::StaticWndEventProc;
	RollupContainerClass.hCursor		= LoadCursor(NULL, IDC_ARROW);
	RollupContainerClass.hIcon			= 0;
	RollupContainerClass.lpszMenuName	= 0;
	RollupContainerClass.hbrBackground	= (HBRUSH)GetSysColorBrush(COLOR_BTNFACE);
	RollupContainerClass.style			= 0;
	RollupContainerClass.cbClsExtra		= 0; 
	RollupContainerClass.cbWndExtra		= 0;
	RollupContainerClass.hIconSm		= 0;

	return RegisterClassEx(&RollupContainerClass);
}


//----------------------------------------------------------//
// CWinSysRollupContainer::UnregisterRollupContainerClass
//----------------------------------------------------------//
//-- Description
// Function to unregister a class for the dummy rollup 
// container window that will appear on a dialog.
// Call this when you're finished with the dialog that had 
// the rollup container on it.
//----------------------------------------------------------//
void CWinSysRollupContainer::UnregisterRollupContainerClass(void)
{
	UnregisterClass(TEXT(WINSYS_ROLLUP_CONTAINER_CLASS_NAME), GetModuleHandle(0));	
}


//----------------------------------------------------------//
// CWinSysRollupContainer::Initialise
//----------------------------------------------------------//
//-- Description
// Construct the rollup container window when the parent 
// dialog is created/shown.
//----------------------------------------------------------//
s32 CWinSysRollupContainer::Construct(HWND hParentDialog, u32 nControlID)
{
	//-- find the control on the dialog that will be our rollupcontainer control
	m_hRollupContainerWnd = GetDlgItem(hParentDialog, nControlID);
	if (!m_hRollupContainerWnd)
	{
		//-- Failed to get a HWND for the rollupcontainer control
		return WINSYS_CONFIG_ROLLUPCONTAINER_NOT_FOUND;
	}

	//-- Add "this" to the data for the container to be used in the event handler
	SetWindowLongPtr(m_hRollupContainerWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

	//-- Start timer to trigger WM_TIMER at ROLLUP_CONTAINER_FRESH_TIME millisecond intervals
	SetTimer(m_hRollupContainerWnd, WINSYS_ROLLUP_CONTAINER_TIMER_ID, WINSYS_ROLLUP_CONTAINER_REFRESH_TIME, NULL);

	//-- record the area boundary of the rollupcontainer control, and store it
	RECT WindowRect;
	GetClientRect(m_hRollupContainerWnd, &WindowRect);

	//-- m_ContainerArea stores the screen size of the rollupcontainer control in rollupcontainer-space
	//-- Origin is 0,0 at top-left corner
	m_ContainerArea.top = 0;
	m_ContainerArea.bottom = WindowRect.bottom - WindowRect.top;
	m_ContainerArea.right = WindowRect.right - WindowRect.left;
	m_ContainerArea.left = 0;

	//-- m_ScrollbarArea stores the rectangle reserved for the scrollbar, in rollupcontainer-space coordinates.
	m_ScrollbarArea.top = m_ContainerArea.top + ROLLUPCONTAINER_SPACING;
	m_ScrollbarArea.bottom = m_ContainerArea.bottom - ROLLUPCONTAINER_SPACING;
	m_ScrollbarArea.right = m_ContainerArea.right - ROLLUPCONTAINER_SPACING;
	m_ScrollbarArea.left = m_ScrollbarArea.right - ROLLUPCONTAINER_SCROLLBAR_WIDTH;	

	//-- m_ScrollbarKnob initially set to fill the entire scrollbar area
	m_ScrollbarKnob.top = m_ScrollbarArea.top;
	m_ScrollbarKnob.bottom = m_ScrollbarArea.bottom;
	m_ScrollbarKnob.right = m_ScrollbarArea.right;
	m_ScrollbarKnob.left = m_ScrollbarArea.left;	 

	m_nCumulativeRollupHeight = 0;

	m_nCurrentScrollOffset = 0;
	m_nMaximumScrollOffset = 0;
	
	m_bDraggingScrollbar = false;
	m_nDraggingYPos = 0;

	//-- Construct all rollups
	for (std::vector<CWinSysRollup>::iterator it = m_Rollups.begin(); it != m_Rollups.end(); ++it)
	{
		if (WINSYS_OK != it->Construct(m_hRollupContainerWnd))
		{
			// Failed to construct rollup
		}
	}

	Update(true);

	return WINSYS_OK;
}


//----------------------------------------------------------//
// CWinSysRollupContainer::ProcessInput
//----------------------------------------------------------//
//-- Description
// Handles input into the client area for the rollup 
// container. The big part of this is handling input for the 
// custom scroll bar, but if the input is not for the scroll
// bar, it will try each child rollup item in turn until
// it finds one that handles the event.
//----------------------------------------------------------//
s32 CWinSysRollupContainer::ProcessInput(u32 nMsg, WPARAM wParam, LPARAM lParam)
{
	s32 nMouseX = GET_X_LPARAM(lParam);
	s32 nMouseY = GET_Y_LPARAM(lParam);

	//-- Test for any mouse event outside the container area
	if (IS_FALSE(IsInsideArea(nMouseX, nMouseY, m_ContainerArea)))
	{
		//-- Mouse is not in rollupcontainer area so abort.
		m_bDraggingScrollbar = false;
		return WINSYS_CONFIG_INPUT_NOT_PROCESSED;
	}

	switch (nMsg)
	{
		case WM_LBUTTONDOWN:
		{
			//-- Is it inside a rollup?
			for (std::vector<CWinSysRollup>::iterator it = m_Rollups.begin(); it != m_Rollups.end(); ++it)
			{
				if (WINSYS_OK == it->ProcessInput(nMsg, wParam, lParam))
				{
					m_bDraggingScrollbar = false;
					return WINSYS_OK;
				}
			}

			if (IS_TRUE(IsInsideArea(nMouseX, nMouseY, m_ScrollbarArea)))
			{
				//-- Inside scrollbar area

				if ( IS_TRUE(IsInsideArea(nMouseX, nMouseY, m_ScrollbarKnob))
					&& (m_nMaximumScrollOffset > 0) )
				{
					m_bDraggingScrollbar = true;
					m_nDraggingYPos = nMouseY;

					Update(true);
					return WINSYS_OK;
				}
			}
		}
		break;

		case WM_LBUTTONUP:
		{
			if (IS_TRUE(m_bDraggingScrollbar))
			{
				if (m_nDraggingYPos != nMouseY)
				{
					s32 nDiff = nMouseY - m_nDraggingYPos;
					s32 nScrollOffset = (s32)(nDiff / m_fScrollBarPixelsPerOffsetUnit);
					nScrollOffset += m_nCurrentScrollOffset;
					m_nCurrentScrollOffset = (u32)CLAMP(0, nScrollOffset, (s32)m_nMaximumScrollOffset);

					m_nDraggingYPos = nMouseY;
				}

				m_bDraggingScrollbar = false;
				Update(true);
			}
			return WINSYS_OK;
		}
		break;

		default:
		{
			if ( IS_TRUE(IsInsideArea(nMouseX, nMouseY, m_ScrollbarArea))
				&& IS_TRUE(m_bDraggingScrollbar) )
			{
				if (m_nDraggingYPos != nMouseY) 
				{
					//-- Dragging
					s32 nDiff = nMouseY - m_nDraggingYPos;
					s32 nScrollOffset = (s32)(nDiff / m_fScrollBarPixelsPerOffsetUnit);
					nScrollOffset += m_nCurrentScrollOffset;
					m_nCurrentScrollOffset = (u32)CLAMP(0, nScrollOffset, (s32)m_nMaximumScrollOffset);

					m_nDraggingYPos = nMouseY;
				}

				Update(true);
			}
			return WINSYS_OK;
		}
		break;
	}

	//-- Nothing handled the input
	return WINSYS_CONFIG_INPUT_NOT_PROCESSED;
}


//----------------------------------------------------------//
// CWinSysRollupContainer::Update
//----------------------------------------------------------//
//-- Description
// Update function for the rollup container. Updates all
// child rollups, repostions as necessary, updates the 
// scroll bar determines if any rollups need redrawing.
//----------------------------------------------------------//
s32 CWinSysRollupContainer::Update(bool bForceRedraw)
{
	s32 nPosition = ROLLUPCONTAINER_SPACING - m_nCurrentScrollOffset;

	for (std::vector<CWinSysRollup>::iterator it = m_Rollups.begin(); it != m_Rollups.end(); ++it)
	{
		bForceRedraw |= it->Update();
		it->Reposition(&nPosition);
	}	
 
	m_nCumulativeRollupHeight = nPosition + m_nCurrentScrollOffset + ROLLUPCONTAINER_SPACING;

	//-- Now we should have a cumulative height for all rollups
	//-- Calculate the maximum scroll extent possible (the range that can't fit in the container area)
	s32 nDiff = m_nCumulativeRollupHeight - (m_ContainerArea.bottom - m_ContainerArea.top);
	m_nMaximumScrollOffset = MAX(nDiff, 0);

	//-- Make sure the current scroll extent is within the calculated range
	m_nCurrentScrollOffset = MIN(m_nCurrentScrollOffset, m_nMaximumScrollOffset);

	//-- Calculate the size of a scrollbar unit in relation to the range
	m_fScrollBarPixelsPerOffsetUnit = (f32)(m_ScrollbarArea.bottom - m_ScrollbarArea.top) / (f32)(m_nCumulativeRollupHeight + m_nMaximumScrollOffset);

	//-- Resize the scrollbar knob
	m_ScrollbarKnob.top = m_ScrollbarArea.top + (u32)(m_fScrollBarPixelsPerOffsetUnit * (f32)m_nCurrentScrollOffset);
	m_ScrollbarKnob.top = MAX(m_ScrollbarKnob.top, m_ScrollbarArea.top);
	m_ScrollbarKnob.bottom = m_ScrollbarKnob.top + (u32)(m_fScrollBarPixelsPerOffsetUnit * (f32)m_nCumulativeRollupHeight);
	m_ScrollbarKnob.bottom = MIN(m_ScrollbarKnob.bottom, m_ScrollbarArea.bottom);

	if (IS_TRUE(bForceRedraw))
	{
		InvalidateRect(m_hRollupContainerWnd, NULL, true);
		UpdateWindow(m_hRollupContainerWnd);
	}

	return WINSYS_OK;
}


//----------------------------------------------------------//
// CWinSysRollupContainer::Render
//----------------------------------------------------------//
//-- Description
// Redraws the rollup container and all children. Clipping is
// done automatically.
//----------------------------------------------------------//
s32 CWinSysRollupContainer::Render(void)
{
	PAINTSTRUCT ps;
	HDC hDC = BeginPaint(m_hRollupContainerWnd, &ps); 

	// Render all rollups (actually just the title bar of each rollup)
	for (std::vector<CWinSysRollup>::iterator it = m_Rollups.begin(); it != m_Rollups.end(); ++it)
	{
		if (WINSYS_OK != it->Render(hDC))
		{
			//-- Failed to render a rollup
		}
	}	

	//-- Render the scrollbar
	//-- Grey rectangle first
	if (IS_TRUE(m_bDraggingScrollbar))
	{
		FillRect(hDC, &m_ScrollbarKnob, (HBRUSH)GetStockObject(GRAY_BRUSH));
	}
	else
	{
		FillRect(hDC, &m_ScrollbarKnob, (HBRUSH)GetStockObject(LTGRAY_BRUSH));
	}

	POINT point[3];
	
	//-- Shadow edges (black along bottom and right)
	HPEN hOldPen = (HPEN)SelectObject(hDC, GetStockObject(BLACK_PEN));
	point[0].x = m_ScrollbarKnob.right;
	point[0].y = m_ScrollbarKnob.top;
	point[1].x = m_ScrollbarKnob.right;
	point[1].y = m_ScrollbarKnob.bottom;
	point[2].x = m_ScrollbarKnob.left;
	point[2].y = m_ScrollbarKnob.bottom;
	Polyline(hDC, point, 3);

	//-- Highlight edges (white along top and left)
	SelectObject(hDC, GetStockObject(WHITE_PEN));
	point[1].x = m_ScrollbarKnob.left;
	point[1].y = m_ScrollbarKnob.top;
	Polyline(hDC, point, 3);

	SelectObject(hDC, hOldPen);
	EndPaint(m_hRollupContainerWnd, &ps);

	return WINSYS_OK;
}


//----------------------------------------------------------//
// CWinSysRollupContainer::AddRollup
//----------------------------------------------------------//
//-- Description
// Adds a new rollup to the set of rollups inside the 
// container. NOTE the items will appear in reverse order
// that they are added, which is just down to me being lazy
// with linked lists.
//----------------------------------------------------------//
s32 CWinSysRollupContainer::AddRollup(const s8* strName, s32 nDialogID, CWinSysRollup::State::Enum eInitialState)
{
	m_Rollups.push_back(CWinSysRollup(this, strName, nDialogID, eInitialState));

	//-- Success
	return WINSYS_OK;
} 


//----------------------------------------------------------//
// CWinSysRollupContainer::IsInsideArea
//----------------------------------------------------------//
//-- Description
// Test to see if a coordinate is inside a given rectangle.
//----------------------------------------------------------//
bool	CWinSysRollupContainer::IsInsideArea(s32 nMouseX, s32 nMouseY, RECT& area)
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



//----------------------------------------------------------//
// CWinSysRollupContainer::GetContainerArea
//----------------------------------------------------------//
//-- Description
// Return the container area rectangle.
//----------------------------------------------------------//
RECT& CWinSysRollupContainer::GetContainerArea(void) 
{ 
	return m_ContainerArea; 
}


//----------------------------------------------------------//
// CWinSysRollupContainer::CancelScroll
//----------------------------------------------------------//
//-- Description
// Force cancel any scrollbar activity
//----------------------------------------------------------//
void CWinSysRollupContainer::CancelScroll(void)
{
	if (IS_TRUE(m_bDraggingScrollbar))
	{
		m_bDraggingScrollbar = false;
		Update(true);
	}
}



//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//
