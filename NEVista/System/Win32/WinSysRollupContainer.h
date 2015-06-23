#ifndef _WINSYS_ROLLUP_CONTAINER_H_
#define _WINSYS_ROLLUP_CONTAINER_H_

//----------------------------------------------------------//
// WINSYSROLLUPCONTAINER.H
//----------------------------------------------------------//
//-- Description			
// Class for a rollup container object, made from a dummy
// window object inside a dialog. The container represents
// the area inside which child rollups exist.
//----------------------------------------------------------//


#include "../Ini.h"
#include <vector>


//----------------------------------------------------------//
// DEFINES
//----------------------------------------------------------//


#define ROLLUPCONTAINER_SPACING					(2)
#define ROLLUPCONTAINER_SCROLLBAR_WIDTH			(5)
#define ROLLUPCONTAINER_SCROLLBAR_SPACING		(ROLLUPCONTAINER_SPACING + ROLLUPCONTAINER_SCROLLBAR_WIDTH + ROLLUPCONTAINER_SPACING)


//----------------------------------------------------------//
// ENUMS
//----------------------------------------------------------//

//----------------------------------------------------------//
// FORWARD REFERENCES
//----------------------------------------------------------//


class CWinSysConfigDialog;
class CWinSysRollup;


//----------------------------------------------------------//
// STRUCTS
//----------------------------------------------------------//

//----------------------------------------------------------//
// CLASSES
//----------------------------------------------------------//


class CWinSysRollupContainer
{
	public:

		CWinSysRollupContainer();
		~CWinSysRollupContainer();

		s32				Initialise(CWinSysConfigDialog* pParent);
		s32				Shutdown(void);
		s32				Construct(HWND hParentDialog, u32 nControlID);
		s32				Update(bool bForceRedraw);
		s32				Render(void);
		s32				ProcessInput(UINT nMsg, WPARAM wParam, LPARAM lParam);

		RECT&			GetContainerArea(void);
		void			CancelScroll(void);

		CWinSysConfigDialog* GetParentConfigDialog(void);

		static s32		RegisterRollupContainerClass(void);
		static void		UnregisterRollupContainerClass(void);

		static LRESULT CALLBACK StaticWndEventProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam);
		LRESULT			WndEventProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam); 

	private:

		s32				AddRollup(const s8* strName, s32 nDialogID, CWinSysRollup::State::Enum eInitialState = CWinSysRollup::State::Unrolled);
		bool			IsInsideArea(s32 nMouseX, s32 nMouseY, RECT& area);

		HWND			m_hRollupContainerWnd;
		CWinSysConfigDialog* m_pParent;

		RECT			m_ContainerArea;
		RECT			m_ScrollbarArea;
		RECT			m_ScrollbarKnob;

		u32				m_nCumulativeRollupHeight;

		f32				m_fScrollBarPixelsPerOffsetUnit;
		u32				m_nCurrentScrollOffset;
		u32				m_nMaximumScrollOffset;

		s32				m_nDraggingYPos;
		bool			m_bDraggingScrollbar;

		std::vector<CWinSysRollup> m_Rollups;
};


//----------------------------------------------------------//
// EXTERNALS
//----------------------------------------------------------//

//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//

#endif