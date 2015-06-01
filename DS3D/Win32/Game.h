#ifndef _GAMELOOP_H_
#define _GAMELOOP_H_
#pragma once

//----------------------------------------------------------//
// GAME.H
//----------------------------------------------------------//
//-- Description
// Main game initialise/shutdown/loop and main()
//----------------------------------------------------------//

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

//----------------------------------------------------------//
// EXTERNALS
//----------------------------------------------------------//

const s8* Game_Title(void);
const s8* Game_Version(void);

bool Game_Initialise(void);
bool Game_Shutdown(void);
bool Game_Main(void);


//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//
#endif //_GAMELOOP_H_