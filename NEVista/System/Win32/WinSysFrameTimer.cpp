//----------------------------------------------------------//
// WINSYSFRAMETIMER.CPP
//----------------------------------------------------------//
//-- Description			
// High resolution game timer, for timing frames.
//----------------------------------------------------------//


#include "WinSysIncludes.h"
#include "Types.h"
#include "SysString.h"


//----------------------------------------------------------//
// DEFINES
//----------------------------------------------------------//

//----------------------------------------------------------//
// GLOBALS
//----------------------------------------------------------//

//----------------------------------------------------------//
// CWinSysFrameTimer::CWinSysFrameTimer
//----------------------------------------------------------//
//-- Description			
// Constructor for timer object
//----------------------------------------------------------//
CWinSysFrameTimer::CWinSysFrameTimer()
{
}

//----------------------------------------------------------//
// CWinSysFrameTimer::~CWinSysFrameTimer
//----------------------------------------------------------//
//-- Description			
// Destructor for timer object
//----------------------------------------------------------//
CWinSysFrameTimer::~CWinSysFrameTimer()
{
}


//----------------------------------------------------------//
// CWinSysFrameTimer::Initialise
//----------------------------------------------------------//
//-- Description			
// Initialises the hi-res frame timer
//----------------------------------------------------------//
s32 CWinSysFrameTimer::Initialise(void)
{
	gDebugLog.Printf("Hi-Res Frame Timer Initialisation");
	SCOPED_LOG_INDENT(gDebugLog);

	m_fFrameTime			= 0.0f;
	m_fFrameMultiplier		= 0.0f;
	
	m_fFPS					= 0.0f;
	
	m_fGameTime				= 0.0f;

	m_LoRes.m_nGameStartTime = GetTickCount();
	m_LoRes.m_nLastTime = m_LoRes.m_nGameStartTime;

	LARGE_INTEGER nClockFrequency;
	if (QueryPerformanceFrequency(&nClockFrequency))
	{
		//-- hi-res timer available
		gDebugLog.Printf("Hi-Res frame timer is available for use.");

		QueryPerformanceCounter(&m_HiRes.m_nLastTime);
	}
	else
	{
		//-- no hi-res timer unavailable, use lo-res millisecond timer instead
		gDebugLog.Printf("Hi-Res frame timer is not available. Switching to Low-Res frame timer (accuracy will be affected).");
	}

	gDebugLog.Printf("Initialisation completed. (OK)");

	return WINSYS_OK;
}


//----------------------------------------------------------//
// CWinSysFrameTimer::Shutdown
//----------------------------------------------------------//
//-- Description			
// Shuts down the hi-res frame timer
//----------------------------------------------------------//
s32 CWinSysFrameTimer::Shutdown(void)
{
	gDebugLog.Printf("Hi-Res Frame Timer Shutdown:");
	SCOPED_LOG_INDENT(gDebugLog);
	gDebugLog.Printf("Shutdown completed. (OK)");

	return WINSYS_OK;
}


//----------------------------------------------------------//
// CWinSysFrameTimer::Update
//----------------------------------------------------------//
//-- Description			
// Updates the hi-res timer and calculates a few useful
// values for us.
//----------------------------------------------------------//
void CWinSysFrameTimer::Update(void)
{
	//-- Calculate the length of the frame (well, it's the previous frame technically)
	DWORD nCurrentTicks	= GetTickCount();

	m_fGameTime = 0.001f * (f32)(nCurrentTicks - m_LoRes.m_nGameStartTime);

	LARGE_INTEGER nClockFrequency;
	if (QueryPerformanceFrequency(&nClockFrequency))
	{
		LARGE_INTEGER nCurrentHiTime;
		QueryPerformanceCounter(&nCurrentHiTime);

		m_HiRes.m_fReciprocalClockFrequency = 1.0f / (f32)nClockFrequency.QuadPart;

		m_fFrameTime = 1000.0f 
						* ((f32)nCurrentHiTime.QuadPart - (f32)m_HiRes.m_nLastTime.QuadPart) 
						* m_HiRes.m_fReciprocalClockFrequency;

		m_HiRes.m_nLastTime = nCurrentHiTime;
	}
	else
	{
		m_fFrameTime = (f32)(nCurrentTicks - m_LoRes.m_nLastTime);
		
		m_LoRes.m_nLastTime = nCurrentTicks;
	}

	//-- frame length in ms -> % of 60fps.
	m_fFrameMultiplier = m_fFrameTime * 0.06f;  

	//-- fps
	m_fFPS = 1000.0f / m_fFrameTime;

/*
	//-- every 60 frames update the fps display
	m_nFrameCount++;
if (m_nFrameCount == 60)
	{
		m_nFrameCount = 0;

		s8 strBuffer[64] = {0};
		SysString::Sprintf(strBuffer, 64, GAME_TITLE" - FPS: %.2f, Time: %.1fms, Mul: %.5f", m_fFPS, m_fFrameTime, m_fFrameMultiplier);

		gWinSysGLWindow.SetWindowTitle(strBuffer);
	}
*/
}


//----------------------------------------------------------//
// CWinSysFrameTimer::LockFrameRate
//----------------------------------------------------------//
//-- Description			
// Determines if the frame rate matches the specified 
// parameter. Returns true if yes, otherwise false;
// Application should poll this in a loop for a true return
// before swapping screen buffers.
//----------------------------------------------------------//
s32 CWinSysFrameTimer::LockFrameRate(f32 fTargetFPS) const 
{
	f32 fTargetFPSAsMilliseconds = 1000.0f / fTargetFPS;
	f32 fDeltaTime = 0.0f;

	if (m_HiRes.m_fReciprocalClockFrequency > 0.0f)
	{
		LARGE_INTEGER currentHiTime;
		QueryPerformanceCounter(&currentHiTime);
		
		fDeltaTime = 1000.0f
					* ((f32)currentHiTime.QuadPart - (f32)m_HiRes.m_nLastTime.QuadPart)
					* m_HiRes.m_fReciprocalClockFrequency;
	}
	else
	{
		fDeltaTime = (f32)(GetTickCount() - m_LoRes.m_nLastTime);
	}

	if ((fDeltaTime + m_fFrameTime) < fTargetFPSAsMilliseconds)
	{
		return WINSYS_TIMER_FPS_TARGET_NOT_REACHED;
	}

	return WINSYS_OK;
}


//----------------------------------------------------------//
// CWinSysFrameTimer::GetFPS
//----------------------------------------------------------//
//-- Description			
// Returns the current frame rate.
//----------------------------------------------------------//
f32	CWinSysFrameTimer::GetFPS(void) const 
{
	return m_fFPS;
}


//----------------------------------------------------------//
// CWinSysFrameTimer::GetFrameTime
//----------------------------------------------------------//
//-- Description			
// Returns the length of the frame, in milliseconds.
//----------------------------------------------------------//
f32	CWinSysFrameTimer::GetFrameTime(void) const 
{
	return m_fFrameTime;
}


//----------------------------------------------------------//
// CWinSysFrameTimer::GetFrameMultiplier
//----------------------------------------------------------//
//-- Description			
// Returns a multiplier determined by the length of the frame.
// 1.0 = 60 fps, 2.0 = 30 fps, 3.0 = 20 fps, etc.
// In practice, only useful if the frame takes longer than the
// frame rate lock, because anything under the frame-rate lock
// is padded TO the frame rate lock.
//----------------------------------------------------------//
f32 CWinSysFrameTimer::GetFrameMultiplier(void) const 
{
	return m_fFrameMultiplier;
}


//----------------------------------------------------------//
// CWinSysFrameTimer::GetGameTime
//----------------------------------------------------------//
//-- Description			
// Gets the time (in SECONDS) since the application started.
//----------------------------------------------------------//
f32	CWinSysFrameTimer::GetGameTime(void) const 
{
	return m_fGameTime;
}


//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//
