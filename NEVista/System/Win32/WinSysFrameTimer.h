#ifndef _WINSYS_FRAME_TIMER_H_
#define _WINSYS_FRAME_TIMER_H_
#pragma once

//----------------------------------------------------------//
// WINSYSTIMER.H
//----------------------------------------------------------//
//-- Description			
// High resolution game timer
//----------------------------------------------------------//


#include "Types.h"


//----------------------------------------------------------//
// DEFINES
//----------------------------------------------------------//

//----------------------------------------------------------//
// ENUMS
//----------------------------------------------------------//

//----------------------------------------------------------//
// STRUCTS
//----------------------------------------------------------//

//----------------------------------------------------------//
// CLASSES
//----------------------------------------------------------//


class CWinSysFrameTimer
{
	public:

		CWinSysFrameTimer();
		~CWinSysFrameTimer();

		s32					Initialise(void);
		s32					Shutdown(void);

		void				Update(void);

		s32					LockFrameRate(f32 fTargetFPS) const;
	
		f32					GetFPS(void) const;						//-- current FPS
		f32					GetFrameTime(void) const;				//-- length of the frame in milliseconds
		f32					GetFrameMultiplier(void) const;			//-- 1.0f = 60 fps, 2.0f = 30 fps, etc
		f32					GetGameTime(void) const;				//-- total game time in SECONDS

	private:

		struct sWinSysTimerHiRes
		{
			LARGE_INTEGER	m_nLastTime;
			f32				m_fReciprocalClockFrequency;
		};

		struct sWinSysTimerLoRes
		{
			DWORD			m_nGameStartTime;
			DWORD			m_nLastTime;
		}; 

		sWinSysTimerHiRes	m_HiRes;
		sWinSysTimerLoRes	m_LoRes;
		
		f32					m_fFrameTime;
		f32					m_fFrameMultiplier;

		f32					m_fFPS;

		f32					m_fGameTime;
};


//----------------------------------------------------------//
// EXTERNALS
//----------------------------------------------------------//

//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//

#endif //_WINSYS_FRAME_TIMER_H_