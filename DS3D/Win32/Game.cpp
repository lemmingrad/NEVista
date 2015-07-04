//----------------------------------------------------------//
// GAME.CPP
//----------------------------------------------------------//
//-- Description
// Main game initialise/shutdown/loop and main()
//----------------------------------------------------------//


#include "Game.h"
#include "SystemIncludes.h"
#include "Win32/WinSysIncludes.h"
#include "VersionInfo.h"
#include "PacketSerializer.h"

#include "GL/gl.h"
#include "GL/glu.h"


//----------------------------------------------------------//
// DEFINES
//----------------------------------------------------------//

//----------------------------------------------------------//
// GLOBALS
//----------------------------------------------------------//

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
	u8 buf[1024];

	CPacketSerializer ser(CSerializer::Mode::Serializing, buf, 1024, 0);

	f32 fv2, fv = 100.0f;
	ser.SerializeF32(fv);

	u8 bytes2[20], bytes[20] = "hello test";
	ser.SerializeBytes(bytes, 20);

	s32 nv2, nv = 24000;
	ser.SerializeS32(nv);

	f32 breakhere = 0.0f;

	CPacketSerializer serb(CSerializer::Mode::Deserializing, buf, 1024, 0);

	serb.SerializeF32(fv2);
	serb.SerializeBytes(bytes2, 20);
	serb.SerializeS32(nv2);

	FixedString<5> fcc;
	SysString::FourCC(fcc.Buffer(), fcc.Size(), 'intb');

	breakhere = 0.0f;




	return true;
}


//----------------------------------------------------------//
// Game_Shutdown
//----------------------------------------------------------//
// Game shutdown. Occurs after main loop.
//----------------------------------------------------------//
bool Game_Shutdown(void)
{
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
