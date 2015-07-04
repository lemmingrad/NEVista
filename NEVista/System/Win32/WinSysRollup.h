#ifndef _WINSYS_ROLLUP_H_
#define _WINSYS_ROLLUP_H_
#pragma once

//----------------------------------------------------------//
// WINSYSROLLUP.H
//----------------------------------------------------------//
//-- Description			
// Class for a rollup control made from a windows dialog.
// Rollups can be added to a rollup container and displayed
// inside another dialog.
//----------------------------------------------------------//


#include "Types.h"
#include "SysString.h"


//----------------------------------------------------------//
// DEFINES
//----------------------------------------------------------//


#define WINSYS_ROLLUP_NAME_LENGTH (32)


//----------------------------------------------------------//
// ENUMS
//----------------------------------------------------------//

//----------------------------------------------------------//
// STRUCTS
//----------------------------------------------------------//

//----------------------------------------------------------//
// FORWARD REFERENCES
//----------------------------------------------------------//


class CWinSysConfigDialog;
class CWinSysRollupContainer;


//----------------------------------------------------------//
// CLASSES
//----------------------------------------------------------//


class CWinSysRollup
{
	public:
	
		struct State
		{
			enum Enum
			{
				Uninitialised = 0,
				FixedUnrolled,
				RolledUp,
				Unrolling,
				Unrolled,
				RollingUp
			};
		};

		CWinSysRollup(CWinSysRollupContainer* pContainer, const s8* strName, s32 nDialogID, State::Enum eInitialState);
		~CWinSysRollup();

		s32				Construct(HWND hParentContainer);
		bool			Update(void);
		s32				Render(HDC hDC);
		s32				ProcessInput(UINT nMsg, WPARAM wParam, LPARAM lParam);
		
		void			Reposition(s32* pnOffset);

		CWinSysConfigDialog* GetParentConfigDialog(void);

		static  INT_PTR CALLBACK StaticDlgEventProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam);
		INT_PTR			DlgEventProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam);

	private:

		bool			IsInsideArea(s32 nMouseX, s32 nMouseY, RECT& area);

		FixedString<WINSYS_ROLLUP_NAME_LENGTH> m_strName;
		s32				m_nDialogID;

		HWND			m_hRollupWnd;			

		RECT			m_RollupArea;
		RECT			m_ExpanderArrowArea;
		s32				m_nMaxHeight;
		s32				m_nAnimatedHeight;

		State::Enum		m_eState;

		CWinSysRollupContainer* m_pParent;
};


//----------------------------------------------------------//
// EXTERNALS
//----------------------------------------------------------//

//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//

#endif //_WINSYS_ROLLUP_