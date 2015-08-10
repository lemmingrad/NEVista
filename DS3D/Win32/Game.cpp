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
#include "TCPConnection.h"


#include "SimpleBuffer.h"
#include "Packet.h"
#include "PacketSerializer.h"
#include "GL/gl.h"
#include "GL/glu.h"


//----------------------------------------------------------//
// DEFINES
//----------------------------------------------------------//

//----------------------------------------------------------//
// GLOBALS
//----------------------------------------------------------//


CLog				gGameLog;
CTCPConnection		gConnection;


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

	gConnection.Initialise();
	gConnection.Open("192.168.1.36", "5555");



	CSimpleBuffer<1024> buf;

	CPacketSerializer ser(CSerializer::Mode::Serializing, buf.Buffer(), buf.Size());

	CPacket packetIn;
	CPacket::Error::Enum eError = packetIn.Serialize(ser);

	f32 breakhere = 0.0f;

	CPacketSerializer serb(CSerializer::Mode::Deserializing, buf.Buffer(), buf.Size());

	CPacket packetOut;
	eError = packetOut.Serialize(serb);

	breakhere = 0.0f;

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
	gConnection.Shutdown();

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
	// WE WILL BE USING NONE OF THIS. FOR TEST ONLY
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glBegin(GL_TRIANGLES);
		glColor3f(1.0f, 0.0f, 0.0f); // Red
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
