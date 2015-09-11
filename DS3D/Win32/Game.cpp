//----------------------------------------------------------//
// GAME.CPP
//----------------------------------------------------------//
//-- Description
// Main game initialise/shutdown/loop and main()
//----------------------------------------------------------//


#include "Game.h"
#include "VersionInfo.h"
#include "SystemIncludes.h"
#include "Win32/WinSysIncludes.h"
#include "Log.h"
#include "Client.h"

#include "GL/gl.h"
#include "GL/glu.h"


//----------------------------------------------------------//
// DEFINES
//----------------------------------------------------------//

//----------------------------------------------------------//
// GLOBALS
//----------------------------------------------------------//


CLog							gGameLog;
CClient							gClient;


//----------------------------------------------------------//
// WinMain
//----------------------------------------------------------//
int WINAPI WinMain(HINSTANCE hInstance, 
				   HINSTANCE hPrevInstance, 
				   LPSTR lpCmdLine, 
				   int nCmdShow) 
{
	return WinSys_Main(lpCmdLine);
}



//----------------------------------------------------------//
// Game_Initialise
//----------------------------------------------------------//
// Game initialisation. Occurs before main loop.
//----------------------------------------------------------//
const s8* Game_Title(void)
{
	return "TEST";
}


//----------------------------------------------------------//
// Game_Initialise
//----------------------------------------------------------//
// Game initialisation. Occurs before main loop.
//----------------------------------------------------------//
const s8* Game_Version(void)
{
	return GAME_VERSION;
}


//----------------------------------------------------------//
// Game_Initialise
//----------------------------------------------------------//
// Game initialisation. Occurs before main loop.
//----------------------------------------------------------//
bool Game_Initialise(void)
{
	gDebugLog.Printf("Game_Initialise:");
	SCOPED_LOG_INDENT(gDebugLog);

	//-- Initialise a game log
	if (IS_FALSE(gGameLog.Initialise("GameLog.txt")))
	{
		gDebugLog.Printf("Failed to open Game Log.");
		return false;
	}
	gGameLog.Printf("Title: %s, Version: %s", Game_Title(), Game_Version());

	gGameLog.Printf("Game_Initialise:");
	SCOPED_LOG_INDENT(gGameLog);

	CIni ini;
	if (IS_FALSE(ini.Load("Data/config.ini")))
	{
		gDebugLog.Printf("Failed to open config.ini.");
		return false;
	}

	FixedString<128> strServer = ini.ReadString("Network", "Server");
	if (IS_TRUE(strServer.IsEmpty()))
	{
		return false;
	}

	CClient::Error::Enum eClientError = gClient.Initialise();
	if (CClient::Error::Ok != eClientError)
	{
		gDebugLog.Printf("Failed to initialise Client.");
		return false;
	}

	eClientError = gClient.Connect(strServer.Buffer());
	if (CClient::Error::Ok != eClientError)
	{
		gDebugLog.Printf("Failed to begin Client connect to server.");
		return false;
	}

	gGameLog.Printf("Complete (OK)");
	gDebugLog.Printf("Complete (OK)");

	return true;
}


//----------------------------------------------------------//
// Game_Shutdown
//----------------------------------------------------------//
// Game shutdown. Occurs after main loop.
//----------------------------------------------------------//
bool Game_Shutdown(void)
{
	gDebugLog.Printf("Game_Shutdown:");
	SCOPED_LOG_INDENT(gDebugLog);
	gGameLog.Printf("Game_Shutdown:");
	SCOPED_LOG_INDENT(gGameLog);

	//-- Shutdown connection
	//-- Should really send a Bye message and wait for safe disconnect,
	//-- but we can't do that here because we're closing the game down completely.
	gClient.Shutdown();

	//-- Close game log
	gGameLog.Printf("Complete (OK)");
	gGameLog.Shutdown();

	gDebugLog.Printf("Complete (OK)");

	return true;
}


//----------------------------------------------------------//
// Game_Main
//----------------------------------------------------------//
// The game's main update loop. NEVista updates may occur
// before Game_Main (input, etc) and after (render, flush, swap). 
//----------------------------------------------------------//
bool Game_Main(void)
{
	gClient.Update();


	// WE WILL BE USING NONE OF THIS. FOR TEST ONLY
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	f32 col[3];
	col[0] = 1.0f;
	col[1] = 0.0f;
	col[2] = 0.0f;

	glBegin(GL_TRIANGLES);
		glColor3fv(col); 
		glVertex2f(-0.5f, -0.5f);    // x, y
		glVertex2f( 0.5f, -0.5f);
		glVertex2f( 0.5f,  0.5f);
	glEnd();

	glFlush();

	return true;
}


//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//
