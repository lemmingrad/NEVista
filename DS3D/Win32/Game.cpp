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
#include "TCPConnector.h"

#include "Network.h"
#include "Message.h"
#include "Messages/MsgMotd.h"
#include "Messages/MsgClientKeyExchange.h"
#include "Messages/MsgServerKeyExchange.h"
#include "Messages/MsgBye.h"


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
CTCPConnection*		g_pConnection;

TMessageList		gRecvMessageList;
TMessageList		gSendMessageList;


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
	if (strServer.IsEmpty())
	{
		return false;
	}

	s8* pNext;
	s8* strIP = SysString::Strtok(strServer.Buffer(), ":", pNext);
	s8* strPort = SysString::Strtok(NULL, "\r\n", pNext);
	if ( IS_TRUE(SysString::IsEmpty(strIP))
		|| IS_TRUE(SysString::IsEmpty(strPort)) )
	{
		return false;
	}

	CTCPConnector connector;
	CTCPConnector::Error::Enum eE = connector.ConnectNonblocking(strIP, strPort);
	if (CTCPConnector::Error::InProgress == eE)
	{
		CTCPConnector::Result r = connector.Update();
		while (r.m_eError == CTCPConnector::Error::InProgress)
		{
			r = connector.Update();
		}
		g_pConnection = r.m_pConnection;
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
	//-- Should really sent a 'bye bye' message, which will close the connection nicely. 
	//-- Close() should only be used for bad cases.
	g_pConnection->Close(CTCPConnection::Error::Ok);
	SAFE_DELETE(g_pConnection);

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

	f32 col[3];
	col[0] = 1.0f;
	col[1] = 0.0f;
	col[2] = 0.0f;


	g_pConnection->UpdateRecv(gRecvMessageList, gSendMessageList);

	while (gRecvMessageList.size() > 0)
	{
//		gClientLoginManager.Update(recvList, sendList);

		SysSmartPtr<CMessage> message = gRecvMessageList.front();
		
		switch (message->GetType())
		{
			case CMsgMotd::kType:
			{
//				if (CClientLoginManager::Error::Ok == gClientLoginManager->ReceivedMotd())
				{
					//-- Ok.
					CMessage* pM = message.ptr();
					CMsgMotd* pMotd = (CMsgMotd*)pM;

					SysDebugPrintf("Received MOTD:\n");
					SysDebugPrintf("%s\n", pMotd->GetText());

//					CMsgClientServerKey* pKeyM = new CMsgClientServerKey();
//					pKeyM->
				}
//				else
//				{
//					//-- Error.
//				}
			}
			break;

			default:
			break;
		}

		gRecvMessageList.pop_front();
	}

	g_pConnection->UpdateSend(gSendMessageList);



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
