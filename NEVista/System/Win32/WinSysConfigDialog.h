#ifndef _WINSYS_CONFIG_DIALOG_H_
#define _WINSYS_CONFIG_DIALOG_H_
#pragma once

//----------------------------------------------------------//
// WINSYSCONFIGDIALOG.H
//----------------------------------------------------------//
//-- Description			
// Config dialog for setting up display settings.
// Either triggered automatically by config data, or 
// triggered whenever an invalid display mode is set. 
//----------------------------------------------------------//


#include "Types.h"
#include "FixedString.h"
#include "Ini.h"
#include "WinSysRollupContainer.h"


//----------------------------------------------------------//
// DEFINES
//----------------------------------------------------------//


#define WINSYS_CONFIG_TITLE_MAX_SIZE		(64)
#define WINSYS_CONFIG_VERSION_MAX_SIZE		(64)


//----------------------------------------------------------//
// ENUMS
//----------------------------------------------------------//

//----------------------------------------------------------//
// FORWARD REFERENCES
//----------------------------------------------------------//

//----------------------------------------------------------//
// STRUCTS
//----------------------------------------------------------//

//----------------------------------------------------------//
// CLASSES
//----------------------------------------------------------//


class CWinSysConfigDialog
{
	public:

		struct DesiredWindowMode
		{
			enum Enum 
			{
				Windowed = 0,
				Borderless,
				Fullscreen,
				MAX
			};

			s32		m_nStringID;
		};

		struct DesiredResolution
		{
			enum Enum
			{
				Res640x480 = 0,
				Res800x600,
				Res1024x768,
				Res1280x720,
				Res1280x800,
				Res1366x768,
				Res1440x900,
				Res1600x900,
				Res1680x1050,
				Res1920x1080,
				MAX
			};

			s32		m_nStringID;
			s32		m_nWidth;
			s32		m_nHeight;
		};

		struct DesiredBPP
		{
			enum Enum
			{
				Bpp16 = 0,
				Bpp32,
				MAX
			};

			s32		m_nStringID;
			s32		m_nBPP;
		};

		static DesiredWindowMode	sm_WindowModeTable[];
		static DesiredResolution	sm_ResolutionTable[];
		static DesiredBPP			sm_BPPTable[];

		CWinSysConfigDialog();
		~CWinSysConfigDialog();

		s32							Initialise(const s8* strTitle, const s8* strVersion);
		s32							Shutdown(void);
		s32							Update(void);
		s32							Show(void);

		static  INT_PTR CALLBACK	StaticDlgEventProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam);
		INT_PTR						DlgEventProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam);

		bool						ShowAtStartup(void);
		void						ChangeShowAtStartup(bool bShow);

		DesiredResolution::Enum		GetDesiredResolution(void);
		void						SetDesiredResolution(DesiredResolution::Enum eResolution);
		s32							GetDesiredResolutionWidth(void);
		s32							GetDesiredResolutionHeight(void);

		DesiredBPP::Enum			GetDesiredBPP(void);
		void						SetDesiredBPP(DesiredBPP::Enum eBPP);
		s32							GetDesiredBPPValue(void);

		DesiredWindowMode::Enum		GetDesiredWindowMode(void);
		void						SetDesiredWindowMode(DesiredWindowMode::Enum eWM);

		const s8*					GetServerAddress(void);
		void						SetServerAddress(const s8* strAddress);
		const s8*					GetServerLoginEmail(void);
		void						SetServerLoginEmail(const s8* strEmail);
		const s8*					GetServerLoginPassword(void);
		void						SetServerLoginPassword(const s8* strPassword);

	private:

		CWinSysRollupContainer		m_RollupContainer;

		CIni						m_ConfigIni;

		FixedString<WINSYS_CONFIG_TITLE_MAX_SIZE>		m_strTitle;
		FixedString<WINSYS_CONFIG_VERSION_MAX_SIZE>		m_strVersion;

		bool						m_bRegistered;
};


//----------------------------------------------------------//
// EXTERNALS
//----------------------------------------------------------//

//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//

#endif //_WINSYS_CONFIG_DIALOG_H_