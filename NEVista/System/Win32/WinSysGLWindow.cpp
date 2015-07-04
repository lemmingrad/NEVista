//----------------------------------------------------------//
// WINSYSGLWINDOW.CPP
//----------------------------------------------------------//
//-- Description			
//----------------------------------------------------------//

#include "WinSysIncludes.h"
#include "Types.h"
#include "Win32/Game.h"
//#include "MathsIncludes.h"
//#include "InputIncludes.h"
//#include "NEVista.h"


//----------------------------------------------------------//
// DEFINES
//----------------------------------------------------------//

//----------------------------------------------------------//
// GLOBALS
//----------------------------------------------------------//


CWinSysGLWindow gWinSysGLWindow;


//----------------------------------------------------------//
// 
//----------------------------------------------------------//
//-- Description			
//----------------------------------------------------------//
CWinSysGLWindow::CWinSysGLWindow()
{
	m_hInstance = NULL;
	m_hWnd = NULL;
	m_hDC = NULL;
	m_hRC = NULL;

	m_eWindowMode = WindowMode::Windowed;
	m_bRegisteredClass = false;
	m_bFulllscreenDisplayActivated = false;

	m_bActiveWindow = false;
}


//----------------------------------------------------------//
// 
//----------------------------------------------------------//
//-- Description			
//----------------------------------------------------------//
CWinSysGLWindow::~CWinSysGLWindow()
{
	Shutdown();
}


//----------------------------------------------------------//
// 
//----------------------------------------------------------//
//-- Description			
//----------------------------------------------------------//
LRESULT CALLBACK CWinSysGLWindow::EventProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		case WM_ACTIVATE:
		{
			gWinSysGLWindow.SetActive((s32)!HIWORD(wParam));
//			gInput.Reset();
			return 0;
		}
		break;

		case WM_SYSCOMMAND:
		{
			switch (lParam)
			{
				case SC_SCREENSAVE:
				case SC_MONITORPOWER:
				{
					//-- stop screensaver or monitor power saving
					return 0;
				}
				break;
				default:
				break;
			}
		}
		break;

		case WM_CLOSE:
		{
			PostQuitMessage(0);
			return 0;
		}
		break;

		case WM_SIZE:
		{
//			gNEVista.View.Resize(LOWORD(lParam), HIWORD(lParam));
			
			return 0;
		}
		break;

		case WM_SETFOCUS:
		case WM_KILLFOCUS:
		{
//			gInput.Reset();

			return 0;
		}

		case WM_KEYDOWN:
		{
			if (wParam == VK_ESCAPE)
			{
				PostQuitMessage(0);
			}
			else
			{
/*				switch(wParam)
				{
					case VK_BACK:			gInput.Key.AddKeyDownEvent(KEY_BACKSPACE);			break;
					case VK_TAB:			gInput.Key.AddKeyDownEvent(KEY_TAB);				break;
					case VK_CLEAR:			gInput.Key.AddKeyDownEvent(KEY_CLEAR);				break;
					case VK_RETURN:			gInput.Key.AddKeyDownEvent(KEY_RETURN);				break;
					case VK_SHIFT:			gInput.Key.AddKeyDownEvent(KEY_SHIFT);				break;
					case VK_CONTROL:		gInput.Key.AddKeyDownEvent(KEY_CTRL);				break;
					case VK_MENU:			gInput.Key.AddKeyDownEvent(KEY_ALT);				break;
					case VK_CAPITAL:		gInput.Key.AddKeyDownEvent(KEY_CAPS_LOCK);			break;
					case VK_SPACE:			gInput.Key.AddKeyDownEvent(KEY_SPACE);				break;

					case VK_PRIOR:			gInput.Key.AddKeyDownEvent(KEY_PAGE_UP);			break;
					case VK_NEXT:			gInput.Key.AddKeyDownEvent(KEY_PAGE_DOWN);			break;
					case VK_END:			gInput.Key.AddKeyDownEvent(KEY_END);				break;
					case VK_HOME:			gInput.Key.AddKeyDownEvent(KEY_HOME);				break;
					case VK_INSERT:			gInput.Key.AddKeyDownEvent(KEY_INSERT);				break;
					case VK_DELETE:			gInput.Key.AddKeyDownEvent(KEY_DELETE);				break;

					case VK_LEFT:			gInput.Key.AddKeyDownEvent(KEY_LEFT_ARROW);			break;
					case VK_RIGHT:			gInput.Key.AddKeyDownEvent(KEY_RIGHT_ARROW);		break;
					case VK_UP:				gInput.Key.AddKeyDownEvent(KEY_UP_ARROW);			break;
					case VK_DOWN:			gInput.Key.AddKeyDownEvent(KEY_DOWN_ARROW);			break;
		
					case VK_SNAPSHOT:		gInput.Key.AddKeyDownEvent(KEY_PRINT_SCREEN);		break;
					case VK_SCROLL:			gInput.Key.AddKeyDownEvent(KEY_SCROLL_LOCK);		break;
					case VK_PAUSE:			gInput.Key.AddKeyDownEvent(KEY_PAUSE);				break;

					case 'A':				gInput.Key.AddKeyDownEvent(KEY_A);					break;
					case 'B':				gInput.Key.AddKeyDownEvent(KEY_B);					break;
					case 'C':				gInput.Key.AddKeyDownEvent(KEY_C);					break;
					case 'D':				gInput.Key.AddKeyDownEvent(KEY_D);					break;
					case 'E':				gInput.Key.AddKeyDownEvent(KEY_E);					break;
					case 'F':				gInput.Key.AddKeyDownEvent(KEY_F);					break;
					case 'G':				gInput.Key.AddKeyDownEvent(KEY_G);					break;
					case 'H':				gInput.Key.AddKeyDownEvent(KEY_H);					break;
					case 'I':				gInput.Key.AddKeyDownEvent(KEY_I);					break;
					case 'J':				gInput.Key.AddKeyDownEvent(KEY_J);					break;
					case 'K':				gInput.Key.AddKeyDownEvent(KEY_K);					break;
					case 'L':				gInput.Key.AddKeyDownEvent(KEY_L);					break;
					case 'M':				gInput.Key.AddKeyDownEvent(KEY_M);					break;
					case 'N':				gInput.Key.AddKeyDownEvent(KEY_N);					break;
					case 'O':				gInput.Key.AddKeyDownEvent(KEY_O);					break;
					case 'P':				gInput.Key.AddKeyDownEvent(KEY_P);					break;
					case 'Q':				gInput.Key.AddKeyDownEvent(KEY_Q);					break;
					case 'R':				gInput.Key.AddKeyDownEvent(KEY_R);					break;
					case 'S':				gInput.Key.AddKeyDownEvent(KEY_S);					break;
					case 'T':				gInput.Key.AddKeyDownEvent(KEY_T);					break;
					case 'U':				gInput.Key.AddKeyDownEvent(KEY_U);					break;
					case 'V':				gInput.Key.AddKeyDownEvent(KEY_V);					break;
					case 'W':				gInput.Key.AddKeyDownEvent(KEY_W);					break;
					case 'X':				gInput.Key.AddKeyDownEvent(KEY_X);					break;
					case 'Y':				gInput.Key.AddKeyDownEvent(KEY_Y);					break;
					case 'Z':				gInput.Key.AddKeyDownEvent(KEY_Z);					break;

					case '0':				gInput.Key.AddKeyDownEvent(KEY_0);					break;
					case '1':				gInput.Key.AddKeyDownEvent(KEY_1);					break;
					case '2':				gInput.Key.AddKeyDownEvent(KEY_2);					break;
					case '3':				gInput.Key.AddKeyDownEvent(KEY_3);					break;
					case '4':				gInput.Key.AddKeyDownEvent(KEY_4);					break;
					case '5':				gInput.Key.AddKeyDownEvent(KEY_5);					break;
					case '6':				gInput.Key.AddKeyDownEvent(KEY_6);					break;
					case '7':				gInput.Key.AddKeyDownEvent(KEY_7);					break;
					case '8':				gInput.Key.AddKeyDownEvent(KEY_8);					break;
					case '9':				gInput.Key.AddKeyDownEvent(KEY_9);					break;

					case VK_NUMPAD0:		gInput.Key.AddKeyDownEvent(KEY_NUMPAD_0);			break;
					case VK_NUMPAD1:		gInput.Key.AddKeyDownEvent(KEY_NUMPAD_1);			break;
					case VK_NUMPAD2:		gInput.Key.AddKeyDownEvent(KEY_NUMPAD_2);			break;
					case VK_NUMPAD3:		gInput.Key.AddKeyDownEvent(KEY_NUMPAD_3);			break;
					case VK_NUMPAD4:		gInput.Key.AddKeyDownEvent(KEY_NUMPAD_4);			break;
					case VK_NUMPAD5:		gInput.Key.AddKeyDownEvent(KEY_NUMPAD_5);			break;
					case VK_NUMPAD6:		gInput.Key.AddKeyDownEvent(KEY_NUMPAD_6);			break;
					case VK_NUMPAD7:		gInput.Key.AddKeyDownEvent(KEY_NUMPAD_7);			break;
					case VK_NUMPAD8:		gInput.Key.AddKeyDownEvent(KEY_NUMPAD_8);			break;
					case VK_NUMPAD9:		gInput.Key.AddKeyDownEvent(KEY_NUMPAD_9);			break;
					case VK_ADD:			gInput.Key.AddKeyDownEvent(KEY_NUMPAD_ADD);			break;
					case VK_SUBTRACT:		gInput.Key.AddKeyDownEvent(KEY_NUMPAD_SUBTRACT);	break;
					case VK_MULTIPLY:		gInput.Key.AddKeyDownEvent(KEY_NUMPAD_MULTIPLY);	break;
					case VK_DIVIDE:			gInput.Key.AddKeyDownEvent(KEY_NUMPAD_DIVIDE);		break;
					case VK_DECIMAL:		gInput.Key.AddKeyDownEvent(KEY_NUMPAD_DECIMAL);		break;
					case VK_NUMLOCK:		gInput.Key.AddKeyDownEvent(KEY_NUM_LOCK);			break;

					case VK_F1:				gInput.Key.AddKeyDownEvent(KEY_F1);					break;
					case VK_F2:				gInput.Key.AddKeyDownEvent(KEY_F2);					break;
					case VK_F3:				gInput.Key.AddKeyDownEvent(KEY_F3);					break;
					case VK_F4:				gInput.Key.AddKeyDownEvent(KEY_F4);					break;
					case VK_F5:				gInput.Key.AddKeyDownEvent(KEY_F5);					break;
					case VK_F6:				gInput.Key.AddKeyDownEvent(KEY_F6);					break;
					case VK_F7:				gInput.Key.AddKeyDownEvent(KEY_F7);					break;
					case VK_F8:				gInput.Key.AddKeyDownEvent(KEY_F8);					break;
					case VK_F9:				gInput.Key.AddKeyDownEvent(KEY_F9);					break;
					case VK_F10:			gInput.Key.AddKeyDownEvent(KEY_F10);				break;
					case VK_F11:			gInput.Key.AddKeyDownEvent(KEY_F11);				break;
					case VK_F12:			gInput.Key.AddKeyDownEvent(KEY_F12);				break;

					default: break;
				}
*/
			}

			return 0;
		}
		break;

		case WM_SYSKEYDOWN:
		{
			if (wParam == VK_ESCAPE)
			{
				PostQuitMessage(0);
			}
			else
			{
/*
				switch(wParam)
				{
					case VK_BACK:			gInput.Key.AddKeyDownEvent(KEY_BACKSPACE);			break;
					case VK_TAB:			gInput.Key.AddKeyDownEvent(KEY_TAB);				break;
					case VK_CLEAR:			gInput.Key.AddKeyDownEvent(KEY_CLEAR);				break;
					case VK_RETURN:			gInput.Key.AddKeyDownEvent(KEY_RETURN);				break;
					case VK_SHIFT:			gInput.Key.AddKeyDownEvent(KEY_SHIFT);				break;
					case VK_CONTROL:		gInput.Key.AddKeyDownEvent(KEY_CTRL);				break;
					//case VK_MENU:			gInput.Key.AddKeyDownEvent(KEY_ALT);				break;
					case VK_CAPITAL:		gInput.Key.AddKeyDownEvent(KEY_CAPS_LOCK);			break;
					case VK_SPACE:			gInput.Key.AddKeyDownEvent(KEY_SPACE);				break;

					case VK_PRIOR:			gInput.Key.AddKeyDownEvent(KEY_PAGE_UP);			break;
					case VK_NEXT:			gInput.Key.AddKeyDownEvent(KEY_PAGE_DOWN);			break;
					case VK_END:			gInput.Key.AddKeyDownEvent(KEY_END);				break;
					case VK_HOME:			gInput.Key.AddKeyDownEvent(KEY_HOME);				break;
					case VK_INSERT:			gInput.Key.AddKeyDownEvent(KEY_INSERT);				break;
					case VK_DELETE:			gInput.Key.AddKeyDownEvent(KEY_DELETE);				break;

					case VK_LEFT:			gInput.Key.AddKeyDownEvent(KEY_LEFT_ARROW);			break;
					case VK_RIGHT:			gInput.Key.AddKeyDownEvent(KEY_RIGHT_ARROW);		break;
					case VK_UP:				gInput.Key.AddKeyDownEvent(KEY_UP_ARROW);			break;
					case VK_DOWN:			gInput.Key.AddKeyDownEvent(KEY_DOWN_ARROW);			break;
		
					case VK_SNAPSHOT:		gInput.Key.AddKeyDownEvent(KEY_PRINT_SCREEN);		break;
					case VK_SCROLL:			gInput.Key.AddKeyDownEvent(KEY_SCROLL_LOCK);		break;
					case VK_PAUSE:			gInput.Key.AddKeyDownEvent(KEY_PAUSE);				break;

					case 'A':				gInput.Key.AddKeyDownEvent(KEY_A);					break;
					case 'B':				gInput.Key.AddKeyDownEvent(KEY_B);					break;
					case 'C':				gInput.Key.AddKeyDownEvent(KEY_C);					break;
					case 'D':				gInput.Key.AddKeyDownEvent(KEY_D);					break;
					case 'E':				gInput.Key.AddKeyDownEvent(KEY_E);					break;
					case 'F':				gInput.Key.AddKeyDownEvent(KEY_F);					break;
					case 'G':				gInput.Key.AddKeyDownEvent(KEY_G);					break;
					case 'H':				gInput.Key.AddKeyDownEvent(KEY_H);					break;
					case 'I':				gInput.Key.AddKeyDownEvent(KEY_I);					break;
					case 'J':				gInput.Key.AddKeyDownEvent(KEY_J);					break;
					case 'K':				gInput.Key.AddKeyDownEvent(KEY_K);					break;
					case 'L':				gInput.Key.AddKeyDownEvent(KEY_L);					break;
					case 'M':				gInput.Key.AddKeyDownEvent(KEY_M);					break;
					case 'N':				gInput.Key.AddKeyDownEvent(KEY_N);					break;
					case 'O':				gInput.Key.AddKeyDownEvent(KEY_O);					break;
					case 'P':				gInput.Key.AddKeyDownEvent(KEY_P);					break;
					case 'Q':				gInput.Key.AddKeyDownEvent(KEY_Q);					break;
					case 'R':				gInput.Key.AddKeyDownEvent(KEY_R);					break;
					case 'S':				gInput.Key.AddKeyDownEvent(KEY_S);					break;
					case 'T':				gInput.Key.AddKeyDownEvent(KEY_T);					break;
					case 'U':				gInput.Key.AddKeyDownEvent(KEY_U);					break;
					case 'V':				gInput.Key.AddKeyDownEvent(KEY_V);					break;
					case 'W':				gInput.Key.AddKeyDownEvent(KEY_W);					break;
					case 'X':				gInput.Key.AddKeyDownEvent(KEY_X);					break;
					case 'Y':				gInput.Key.AddKeyDownEvent(KEY_Y);					break;
					case 'Z':				gInput.Key.AddKeyDownEvent(KEY_Z);					break;

					case '0':				gInput.Key.AddKeyDownEvent(KEY_0);					break;
					case '1':				gInput.Key.AddKeyDownEvent(KEY_1);					break;
					case '2':				gInput.Key.AddKeyDownEvent(KEY_2);					break;
					case '3':				gInput.Key.AddKeyDownEvent(KEY_3);					break;
					case '4':				gInput.Key.AddKeyDownEvent(KEY_4);					break;
					case '5':				gInput.Key.AddKeyDownEvent(KEY_5);					break;
					case '6':				gInput.Key.AddKeyDownEvent(KEY_6);					break;
					case '7':				gInput.Key.AddKeyDownEvent(KEY_7);					break;
					case '8':				gInput.Key.AddKeyDownEvent(KEY_8);					break;
					case '9':				gInput.Key.AddKeyDownEvent(KEY_9);					break;

					case VK_NUMPAD0:		gInput.Key.AddKeyDownEvent(KEY_NUMPAD_0);			break;
					case VK_NUMPAD1:		gInput.Key.AddKeyDownEvent(KEY_NUMPAD_1);			break;
					case VK_NUMPAD2:		gInput.Key.AddKeyDownEvent(KEY_NUMPAD_2);			break;
					case VK_NUMPAD3:		gInput.Key.AddKeyDownEvent(KEY_NUMPAD_3);			break;
					case VK_NUMPAD4:		gInput.Key.AddKeyDownEvent(KEY_NUMPAD_4);			break;
					case VK_NUMPAD5:		gInput.Key.AddKeyDownEvent(KEY_NUMPAD_5);			break;
					case VK_NUMPAD6:		gInput.Key.AddKeyDownEvent(KEY_NUMPAD_6);			break;
					case VK_NUMPAD7:		gInput.Key.AddKeyDownEvent(KEY_NUMPAD_7);			break;
					case VK_NUMPAD8:		gInput.Key.AddKeyDownEvent(KEY_NUMPAD_8);			break;
					case VK_NUMPAD9:		gInput.Key.AddKeyDownEvent(KEY_NUMPAD_9);			break;
					case VK_ADD:			gInput.Key.AddKeyDownEvent(KEY_NUMPAD_ADD);			break;
					case VK_SUBTRACT:		gInput.Key.AddKeyDownEvent(KEY_NUMPAD_SUBTRACT);	break;
					case VK_MULTIPLY:		gInput.Key.AddKeyDownEvent(KEY_NUMPAD_MULTIPLY);	break;
					case VK_DIVIDE:			gInput.Key.AddKeyDownEvent(KEY_NUMPAD_DIVIDE);		break;
					case VK_DECIMAL:		gInput.Key.AddKeyDownEvent(KEY_NUMPAD_DECIMAL);		break;
					case VK_NUMLOCK:		gInput.Key.AddKeyDownEvent(KEY_NUM_LOCK);			break;

					case VK_F1:				gInput.Key.AddKeyDownEvent(KEY_F1);					break;
					case VK_F2:				gInput.Key.AddKeyDownEvent(KEY_F2);					break;
					case VK_F3:				gInput.Key.AddKeyDownEvent(KEY_F3);					break;
					case VK_F4:				gInput.Key.AddKeyDownEvent(KEY_F4);					break;
					case VK_F5:				gInput.Key.AddKeyDownEvent(KEY_F5);					break;
					case VK_F6:				gInput.Key.AddKeyDownEvent(KEY_F6);					break;
					case VK_F7:				gInput.Key.AddKeyDownEvent(KEY_F7);					break;
					case VK_F8:				gInput.Key.AddKeyDownEvent(KEY_F8);					break;
					case VK_F9:				gInput.Key.AddKeyDownEvent(KEY_F9);					break;
					case VK_F10:			gInput.Key.AddKeyDownEvent(KEY_F10);				break;
					case VK_F11:			gInput.Key.AddKeyDownEvent(KEY_F11);				break;
					case VK_F12:			gInput.Key.AddKeyDownEvent(KEY_F12);				break;

					default: break;
				}
*/
			}

			return 0;
		}
		break;

		case WM_KEYUP:
		case WM_SYSKEYUP:
		{
/*			switch(wParam)
			{
				case VK_ESCAPE:			gInput.Key.AddKeyUpEvent(KEY_ESCAPE);				break;
				case VK_BACK:			gInput.Key.AddKeyUpEvent(KEY_BACKSPACE);			break;
				case VK_TAB:			gInput.Key.AddKeyUpEvent(KEY_TAB);					break;
				case VK_CLEAR:			gInput.Key.AddKeyUpEvent(KEY_CLEAR);				break;
				case VK_RETURN:			gInput.Key.AddKeyUpEvent(KEY_RETURN);				break;
				case VK_SHIFT:			gInput.Key.AddKeyUpEvent(KEY_SHIFT);				break;
				case VK_CONTROL:		gInput.Key.AddKeyUpEvent(KEY_CTRL);					break;
				case VK_MENU:			gInput.Key.AddKeyUpEvent(KEY_ALT);					break;
				case VK_CAPITAL:		gInput.Key.AddKeyUpEvent(KEY_CAPS_LOCK);			break;
				case VK_SPACE:			gInput.Key.AddKeyUpEvent(KEY_SPACE);				break;

				case VK_PRIOR:			gInput.Key.AddKeyUpEvent(KEY_PAGE_UP);				break;
				case VK_NEXT:			gInput.Key.AddKeyUpEvent(KEY_PAGE_DOWN);			break;
				case VK_END:			gInput.Key.AddKeyUpEvent(KEY_END);					break;
				case VK_HOME:			gInput.Key.AddKeyUpEvent(KEY_HOME);					break;
				case VK_INSERT:			gInput.Key.AddKeyUpEvent(KEY_INSERT);				break;
				case VK_DELETE:			gInput.Key.AddKeyUpEvent(KEY_DELETE);				break;

				case VK_LEFT:			gInput.Key.AddKeyUpEvent(KEY_LEFT_ARROW);			break;
				case VK_RIGHT:			gInput.Key.AddKeyUpEvent(KEY_RIGHT_ARROW);			break;
				case VK_UP:				gInput.Key.AddKeyUpEvent(KEY_UP_ARROW);				break;
				case VK_DOWN:			gInput.Key.AddKeyUpEvent(KEY_DOWN_ARROW);			break;
	
				case VK_SNAPSHOT:		gInput.Key.AddKeyUpEvent(KEY_PRINT_SCREEN);			break;
				case VK_SCROLL:			gInput.Key.AddKeyUpEvent(KEY_SCROLL_LOCK);			break;
				case VK_PAUSE:			gInput.Key.AddKeyUpEvent(KEY_PAUSE);				break;

				case 'A':				gInput.Key.AddKeyUpEvent(KEY_A);					break;
				case 'B':				gInput.Key.AddKeyUpEvent(KEY_B);					break;
				case 'C':				gInput.Key.AddKeyUpEvent(KEY_C);					break;
				case 'D':				gInput.Key.AddKeyUpEvent(KEY_D);					break;
				case 'E':				gInput.Key.AddKeyUpEvent(KEY_E);					break;
				case 'F':				gInput.Key.AddKeyUpEvent(KEY_F);					break;
				case 'G':				gInput.Key.AddKeyUpEvent(KEY_G);					break;
				case 'H':				gInput.Key.AddKeyUpEvent(KEY_H);					break;
				case 'I':				gInput.Key.AddKeyUpEvent(KEY_I);					break;
				case 'J':				gInput.Key.AddKeyUpEvent(KEY_J);					break;
				case 'K':				gInput.Key.AddKeyUpEvent(KEY_K);					break;
				case 'L':				gInput.Key.AddKeyUpEvent(KEY_L);					break;
				case 'M':				gInput.Key.AddKeyUpEvent(KEY_M);					break;
				case 'N':				gInput.Key.AddKeyUpEvent(KEY_N);					break;
				case 'O':				gInput.Key.AddKeyUpEvent(KEY_O);					break;
				case 'P':				gInput.Key.AddKeyUpEvent(KEY_P);					break;
				case 'Q':				gInput.Key.AddKeyUpEvent(KEY_Q);					break;
				case 'R':				gInput.Key.AddKeyUpEvent(KEY_R);					break;
				case 'S':				gInput.Key.AddKeyUpEvent(KEY_S);					break;
				case 'T':				gInput.Key.AddKeyUpEvent(KEY_T);					break;
				case 'U':				gInput.Key.AddKeyUpEvent(KEY_U);					break;
				case 'V':				gInput.Key.AddKeyUpEvent(KEY_V);					break;
				case 'W':				gInput.Key.AddKeyUpEvent(KEY_W);					break;
				case 'X':				gInput.Key.AddKeyUpEvent(KEY_X);					break;
				case 'Y':				gInput.Key.AddKeyUpEvent(KEY_Y);					break;
				case 'Z':				gInput.Key.AddKeyUpEvent(KEY_Z);					break;

				case '0':				gInput.Key.AddKeyUpEvent(KEY_0);					break;
				case '1':				gInput.Key.AddKeyUpEvent(KEY_1);					break;
				case '2':				gInput.Key.AddKeyUpEvent(KEY_2);					break;
				case '3':				gInput.Key.AddKeyUpEvent(KEY_3);					break;
				case '4':				gInput.Key.AddKeyUpEvent(KEY_4);					break;
				case '5':				gInput.Key.AddKeyUpEvent(KEY_5);					break;
				case '6':				gInput.Key.AddKeyUpEvent(KEY_6);					break;
				case '7':				gInput.Key.AddKeyUpEvent(KEY_7);					break;
				case '8':				gInput.Key.AddKeyUpEvent(KEY_8);					break;
				case '9':				gInput.Key.AddKeyUpEvent(KEY_9);					break;

				case VK_NUMPAD0:		gInput.Key.AddKeyUpEvent(KEY_NUMPAD_0);				break;
				case VK_NUMPAD1:		gInput.Key.AddKeyUpEvent(KEY_NUMPAD_1);				break;
				case VK_NUMPAD2:		gInput.Key.AddKeyUpEvent(KEY_NUMPAD_2);				break;
				case VK_NUMPAD3:		gInput.Key.AddKeyUpEvent(KEY_NUMPAD_3);				break;
				case VK_NUMPAD4:		gInput.Key.AddKeyUpEvent(KEY_NUMPAD_4);				break;
				case VK_NUMPAD5:		gInput.Key.AddKeyUpEvent(KEY_NUMPAD_5);				break;
				case VK_NUMPAD6:		gInput.Key.AddKeyUpEvent(KEY_NUMPAD_6);				break;
				case VK_NUMPAD7:		gInput.Key.AddKeyUpEvent(KEY_NUMPAD_7);				break;
				case VK_NUMPAD8:		gInput.Key.AddKeyUpEvent(KEY_NUMPAD_8);				break;
				case VK_NUMPAD9:		gInput.Key.AddKeyUpEvent(KEY_NUMPAD_9);				break;
				case VK_ADD:			gInput.Key.AddKeyUpEvent(KEY_NUMPAD_ADD);			break;
				case VK_SUBTRACT:		gInput.Key.AddKeyUpEvent(KEY_NUMPAD_SUBTRACT);		break;
				case VK_MULTIPLY:		gInput.Key.AddKeyUpEvent(KEY_NUMPAD_MULTIPLY);		break;
				case VK_DIVIDE:			gInput.Key.AddKeyUpEvent(KEY_NUMPAD_DIVIDE);		break;
				case VK_DECIMAL:		gInput.Key.AddKeyUpEvent(KEY_NUMPAD_DECIMAL);		break;
				case VK_NUMLOCK:		gInput.Key.AddKeyUpEvent(KEY_NUM_LOCK);				break;

				case VK_F1:				gInput.Key.AddKeyUpEvent(KEY_F1);					break;
				case VK_F2:				gInput.Key.AddKeyUpEvent(KEY_F2);					break;
				case VK_F3:				gInput.Key.AddKeyUpEvent(KEY_F3);					break;
				case VK_F4:				gInput.Key.AddKeyUpEvent(KEY_F4);					break;
				case VK_F5:				gInput.Key.AddKeyUpEvent(KEY_F5);					break;
				case VK_F6:				gInput.Key.AddKeyUpEvent(KEY_F6);					break;
				case VK_F7:				gInput.Key.AddKeyUpEvent(KEY_F7);					break;
				case VK_F8:				gInput.Key.AddKeyUpEvent(KEY_F8);					break;
				case VK_F9:				gInput.Key.AddKeyUpEvent(KEY_F9);					break;
				case VK_F10:			gInput.Key.AddKeyUpEvent(KEY_F10);					break;
				case VK_F11:			gInput.Key.AddKeyUpEvent(KEY_F11);					break;
				case VK_F12:			gInput.Key.AddKeyUpEvent(KEY_F12);					break;

				default: break;
			}
*/
			return 0;
		}
		break;

		case WM_CHAR:
		{
//			gInput.Key.AddKeyChar((s8)wParam);
			return 0;
		}
		break;

		case WM_LBUTTONDOWN:
		{
//			gInput.Mouse.AddMouseButtonDownEvent(CMouseInput::MOUSE_LEFT_BUTTON, CPoint2D(LOWORD(lParam), HIWORD(lParam)));
			return 0;
		}
		break;

		case WM_LBUTTONUP:
		{
//			gInput.Mouse.AddMouseButtonUpEvent(CMouseInput::MOUSE_LEFT_BUTTON, CPoint2D(LOWORD(lParam), HIWORD(lParam)));
			return 0;
		}
		break;

		case WM_MBUTTONDOWN:
		{
//			gInput.Mouse.AddMouseButtonDownEvent(CMouseInput::MOUSE_MIDDLE_BUTTON, CPoint2D(LOWORD(lParam), HIWORD(lParam)));
			return 0;
		}
		break;

		case WM_MBUTTONUP:
		{
//			gInput.Mouse.AddMouseButtonUpEvent(CMouseInput::MOUSE_MIDDLE_BUTTON, CPoint2D(LOWORD(lParam), HIWORD(lParam)));
			return 0;
		}
		break;

		case WM_RBUTTONDOWN:
		{
//			gInput.Mouse.AddMouseButtonDownEvent(CMouseInput::MOUSE_RIGHT_BUTTON, CPoint2D(LOWORD(lParam), HIWORD(lParam)));
			return 0;
		}
		break;

		case WM_RBUTTONUP:
		{
//			gInput.Mouse.AddMouseButtonUpEvent(CMouseInput::MOUSE_RIGHT_BUTTON, CPoint2D(LOWORD(lParam), HIWORD(lParam)));
			return 0;
		}
		break;

		case WM_MOUSEMOVE:
		{
//			gInput.Mouse.AddMouseMoveEvent(CPoint2D(LOWORD(lParam), HIWORD(lParam)));
		}
		break;

		default:
		break;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}


//----------------------------------------------------------//
// CWinSysGLWindow::InitialiseFullscreenMode
//----------------------------------------------------------//
//-- Description
// Attempt to create a fullscreen opengl context
//----------------------------------------------------------//
s32 CWinSysGLWindow::InitialiseFullscreenMode(s32 nWidth, s32 nHeight, s32 nBPP)
{
	s32 nStatus = WINSYS_OK;

	nStatus = ActivateFullscreenDisplay(nWidth, nHeight, nBPP);
	if (WINSYS_OK != nStatus)
	{
		//-- Failed to switch to fullscreen
		return nStatus;
	}

	nStatus = CreateGLWindow(nWidth, nHeight, nBPP, WS_POPUP, WS_EX_APPWINDOW);
	if (WINSYS_OK != nStatus)
	{
		//-- Failed to create window
		RevertFullscreenDisplay();
		return nStatus;
	}

	//-- Success
	m_eWindowMode = WindowMode::Fullscreen;
	return nStatus;
}


//----------------------------------------------------------//
// CWinSysGLWindow::InitialiseBorderlessWindowMode
//----------------------------------------------------------//
//-- Description
// Attempt to create a borderless window context
//----------------------------------------------------------//
s32 CWinSysGLWindow::InitialiseBorderlessWindowMode(s32 nBPP)
{
	RevertFullscreenDisplay();

	s32 nWidth = GetSystemMetrics(SM_CXSCREEN);
	s32 nHeight = GetSystemMetrics(SM_CYSCREEN);

	if ( IS_ZERO(nWidth)
		|| IS_ZERO(nHeight) )
	{
		//-- Bad screen width or height.
		return WINSYS_WINDOW_BAD_WIDTH_OR_HEIGHT;
	}

	s32 nStatus = CreateGLWindow(nWidth, nHeight, nBPP, WS_POPUP, WS_EX_APPWINDOW);
	if (WINSYS_OK != nStatus)
	{
		//-- Failed to create window
		return nStatus;
	}

	//-- Success
	m_eWindowMode = WindowMode::Borderless;
	return nStatus;
}


//----------------------------------------------------------//
// CWinSysGLWindow::InitialiseWindowedMode
//----------------------------------------------------------//
//-- Description			
// Attempt to create a normal opengl window context
//----------------------------------------------------------//
s32 CWinSysGLWindow::InitialiseWindowedMode(s32 nWidth, s32 nHeight, s32 nBPP)
{
	RevertFullscreenDisplay();

	s32 nStatus = CreateGLWindow(nWidth, nHeight, nBPP, WS_OVERLAPPEDWINDOW, WS_EX_APPWINDOW|WS_EX_WINDOWEDGE);
	if (WINSYS_OK != nStatus)
	{
		//-- Failed to create window
		return nStatus;
	}

	//-- Success
	m_eWindowMode = WindowMode::Windowed;
	return nStatus;
}


//----------------------------------------------------------//
// CWinSysGLWindow::Shutdown
//----------------------------------------------------------//
//-- Description	
// Close the active window
//----------------------------------------------------------//
s32 CWinSysGLWindow::Shutdown(void)
{
	gDebugLog.Printf("CWinSysGLWindow::Shutdown:");
	SCOPED_LOG_INDENT(gDebugLog);

	RevertFullscreenDisplay();
	DestroyGLWindow();

	gDebugLog.Printf("Completed. (OK)");

	return WINSYS_OK;
}


//----------------------------------------------------------//
// CWinSysGLWindow::ActivateFullscreenDisplay
//----------------------------------------------------------//
//-- Description			
// Attempt to switch to fullscreen screen mode
//----------------------------------------------------------//
s32 CWinSysGLWindow::ActivateFullscreenDisplay(s32 nWidth, s32 nHeight, s32 nBPP)
{
	DEVMODE dmScreenSettings;	
	dmScreenSettings.dmSize			= sizeof(dmScreenSettings);		//-- Size of the devmode structure
	dmScreenSettings.dmPelsWidth	= nWidth;						//-- Screen width
	dmScreenSettings.dmPelsHeight	= nHeight;						//-- Screen height
	dmScreenSettings.dmBitsPerPel	= nBPP;							//-- Bits per pixel
	dmScreenSettings.dmFields		= DM_PELSWIDTH|DM_PELSHEIGHT|DM_BITSPERPEL;

	//-- Try to set selected mode and get results.  
	//-- NOTE: CDS_FULLSCREEN gets rid of Start Bar.
	if (DISP_CHANGE_SUCCESSFUL != ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN))
	{
		//-- Failed to change to fullscreen
		return WINSYS_WINDOW_FULLSCREEN_SWITCH_FAILED;
	}

	//-- Success
	m_bFulllscreenDisplayActivated = true;
	return WINSYS_OK;
}


//----------------------------------------------------------//
// CWinSysGLWindow::RevertFullscreenDisplay
//----------------------------------------------------------//
//-- Description			
// Revert to windowed screen mode
//----------------------------------------------------------//
s32 CWinSysGLWindow::RevertFullscreenDisplay(void)
{
	if (IS_TRUE(m_bFulllscreenDisplayActivated))
	{
		ChangeDisplaySettings(NULL, 0);
		ShowCursor(true);
	}
	m_bFulllscreenDisplayActivated = false;

	return WINSYS_OK;
}


//----------------------------------------------------------//
// CWinSysGLWindow::CreateGLWindow
//----------------------------------------------------------//
//-- Description			
// Grunt work for creating opengl context
//----------------------------------------------------------//
s32 CWinSysGLWindow::CreateGLWindow(const s32 nWidth, const s32 nHeight, const s32 nBPP, DWORD dwStyle, DWORD dwExStyle)
{
	gDebugLog.Printf("CWinSysGLWindow::CreateGLWindow:");
	SCOPED_LOG_INDENT(gDebugLog);

	if (IS_FALSE(m_bRegisteredClass))
	{
		m_hInstance = GetModuleHandle(0);
 
		WNDCLASSEX GLWindowClass;
		
		GLWindowClass.cbSize			= sizeof(GLWindowClass);
		GLWindowClass.lpszClassName		= Game_Title();									//-- Set the class name
		GLWindowClass.hInstance			= m_hInstance;									//-- Set the window's instance
		GLWindowClass.lpfnWndProc		= (WNDPROC)CWinSysGLWindow::EventProc;			//-- Handle the WNDPROC messages
		GLWindowClass.hCursor			= LoadCursor(NULL, IDC_ARROW);					//-- Load the mouse pointer
		GLWindowClass.hIcon				= LoadIcon(NULL, IDI_WINLOGO);					//-- Load the default icon
		GLWindowClass.lpszMenuName		= 0;											//-- No menu
		GLWindowClass.hbrBackground		= 0;											//-- No background
		GLWindowClass.style				= CS_HREDRAW|CS_VREDRAW|CS_OWNDC;				//-- Redraw the window on resize
		GLWindowClass.cbClsExtra		= 0;											//-- No extra win data
		GLWindowClass.cbWndExtra		= 0;											//-- No extra win data
		GLWindowClass.hIconSm			= 0;

		if (RegisterClassEx(&GLWindowClass))
		{
			m_bRegisteredClass = true;
		}
		else
		{
			//-- Failed to register
			return WINSYS_WINDOW_CLASS_NOT_REGISTERED;
		}
	}
	
	RECT WinRect;   
	WinRect.left   = 0;		
	WinRect.top    = 0;		
	WinRect.right  = nWidth;	
	WinRect.bottom = nHeight;	

	//-- Adjust window to the actual requested size
	AdjustWindowRectEx(&WinRect, dwStyle, false, dwExStyle);

	//-- Create the window
	m_hWnd = CreateWindowEx(dwExStyle,						//-- The extended style for the window
							 Game_Title(),					//-- The class name
							 Game_Title(),					//-- The window's title
							 dwStyle|WS_CLIPSIBLINGS|WS_CLIPCHILDREN,		 
							 0, 0,							//-- The window's position
							 WinRect.right - WinRect.left,	//-- Calculate the window's width
							 WinRect.bottom - WinRect.top,	//-- Calculate the window's height
							 NULL,							//-- No parent window
							 NULL,							//-- No menu needed
							 m_hInstance,					//-- The window's instance
							 NULL);							//-- Nothing to pass to WM_CREATE
	if (!m_hWnd)
	{
		//-- Error, window could not be created.
		gDebugLog.Printf("Window could not be created.");
		return WINSYS_WINDOW_HWND_NOT_CREATED;			
	}

	//-- Attempt the create Device Context
	m_hDC = GetDC(m_hWnd);
	if (!m_hDC)
	{
		//-- Error, Device Context could not be created
		gDebugLog.Printf("Could not create a Device Context.");
		return WINSYS_WINDOW_HDC_NOT_CREATED;								
	}
	gDebugLog.Printf("Device Context created.");

	static PIXELFORMATDESCRIPTOR pfd =	//-- Get the pixel format's description
	{
		sizeof(PIXELFORMATDESCRIPTOR),	//-- Get the size of the structure
		1,								//-- Version number
		PFD_DRAW_TO_WINDOW |			//-- Format must support Windows
		PFD_SUPPORT_OPENGL |			//-- Format must support OpenGL
		PFD_DOUBLEBUFFER,				//-- Must support Double Buffering
		PFD_TYPE_RGBA,					//-- Request a RGBA (red,green,blue,alpha) format
		(u8)nBPP,						//-- Select the bits per pixel
		0, 0, 0, 0, 0, 0,				//-- Color bits ignored
		0,								//-- No alpha buffer
		0,								//-- shift bit ignored
		0,								//-- No accumulation buffer (advanced)
		0, 0, 0, 0,						//-- Accumulation bits ignored
		24,								//-- 24 bit Z-Buffer (Depth Buffer)  
		0,								//-- No Stencil Buffer (advanced)
		0,								//-- No Auxiliary Buffer (advanced)
		PFD_MAIN_PLANE,					//-- The main drawing layer
		0,								//-- Reserved area
		0, 0, 0							//-- Layer masks ignored
	};
	
	//-- Find a matching supported pixel format
	UINT pixelFormat = ChoosePixelFormat(m_hDC, &pfd);
	if (IS_ZERO(pixelFormat))
	{
		//-- Error, no suitable pixel format found
		gDebugLog.Printf("Could not find a suitable pixel format.");
		return WINSYS_WINDOW_PIXELFORMAT_NOT_FOUND;						
	}
	gDebugLog.Printf("Found suitable pixel format.");

	// Set the pixel format
	if (!SetPixelFormat(m_hDC, pixelFormat, &pfd))
	{
		//-- Error, pixel format could not be set
		gDebugLog.Printf("Could not set pixel format.");
		return WINSYS_WINDOW_PIXELFORMAT_NOT_SET;						
	}
	gDebugLog.Printf("Pixel format is set.");

	//-- Attempt to create a Core GL rendering Context
//	m_hRC = wglCreateContextAttribsARB(m_hDC, 0, NULL);

	//-- Create a Compatibility GL Rendering Context if Core failed
	if (!m_hRC)
	{
		m_hRC = wglCreateContext(m_hDC);
	}
	
	if (!m_hRC)
	{
		//-- Error, failed to create a GL rendering context
		gDebugLog.Printf("Could not create an OpenGL Rendering Context.");
		return WINSYS_WINDOW_HRC_NOT_CREATED;		
	}
	gDebugLog.Printf("Created an OpenGL Rendering Context.");

	//-- Make rendering context active
	if (!wglMakeCurrent(m_hDC, m_hRC))
	{
		//-- Error, could not activate rendering context
		gDebugLog.Printf("Could not activate the OpenGL Rendering Context.");
		return WINSYS_WINDOW_HRC_NOT_ACTIVATED;			
	}
	gDebugLog.Printf("Activated the OpenGL Rendering Context.");

	ShowWindow(m_hWnd, SW_SHOW);				//-- Show the window
	SetForegroundWindow(m_hWnd);				//-- Bring this window to the front
	SetFocus(m_hWnd);							//-- Sets the keyboard's focus to the window

	gDebugLog.Printf("Complete. (OK)");

	return WINSYS_OK;
}


//----------------------------------------------------------//
// 
//----------------------------------------------------------//
//-- Description			
//----------------------------------------------------------//
s32 CWinSysGLWindow::DestroyGLWindow(void) 
{
	gDebugLog.Printf("CWinSysGLWindow::DestroyGLWindow:");
	SCOPED_LOG_INDENT(gDebugLog);

	//-- Try to release rendering context
	if (m_hRC) 
	{
		if (wglMakeCurrent(NULL, NULL))
		{
			gDebugLog.Printf("Deactivated OpenGL Rendering Context.");
		}
		else
		{
			gDebugLog.Printf("Could not deactivate OpenGL Rendering Context.");
		}

		if (wglDeleteContext(m_hRC))	
		{
			gDebugLog.Printf("Released OpenGL Rendering Context.");
		}
		else
		{
			gDebugLog.Printf("Could not delete OpenGL Rendering Context.");
		}
	}
	m_hRC = NULL;		

	//-- Try to release the Device Context
	if (m_hDC && ReleaseDC(m_hWnd, m_hDC))	
	{
		gDebugLog.Printf("Released the Device Context.");
	}
	else
	{
		gDebugLog.Printf("Could not release the Device Context.");
	}
	m_hDC = NULL;				

	//-- Try to delete the window
	if (m_hWnd && DestroyWindow(m_hWnd))
	{
		gDebugLog.Printf("Released main window handle.");
	}
	else
	{
		gDebugLog.Printf("Could not release main window handle.");
	}
	m_hWnd = NULL;					
 
	//-- Unregister class
	if (IS_TRUE(m_bRegisteredClass) && UnregisterClass(Game_Title(), m_hInstance))	
	{
		gDebugLog.Printf("Unregistered window class.");
	}
	else
	{
		gDebugLog.Printf("Could not unregister window class.");	
	}
	m_hInstance = NULL;			
	m_bRegisteredClass = false;

	gDebugLog.Printf("Complete. (OK)");

	return WINSYS_OK;
}


//----------------------------------------------------------//
// 
//----------------------------------------------------------//
//-- Description			
//----------------------------------------------------------//
void CWinSysGLWindow::SwapBuffers(void) const
{
	::SwapBuffers(m_hDC);
}


//----------------------------------------------------------//
// 
//----------------------------------------------------------//
//-- Description			
//----------------------------------------------------------//
void CWinSysGLWindow::SetActive(bool bActive)
{
	m_bActiveWindow = bActive;
}


//----------------------------------------------------------//
// 
//----------------------------------------------------------//
//-- Description			
//----------------------------------------------------------//
bool CWinSysGLWindow::IsActive(void) const
{
	return m_bActiveWindow;
}


//----------------------------------------------------------//
// 
//----------------------------------------------------------//
//-- Description			
//----------------------------------------------------------//
CWinSysGLWindow::WindowMode::Enum CWinSysGLWindow::GetWindowMode(void) const
{
	return m_eWindowMode;
}


//----------------------------------------------------------//
// 
//----------------------------------------------------------//
//-- Description			
//----------------------------------------------------------//
void CWinSysGLWindow::SetWindowTitle(const s8* strBuffer)
{
	SetWindowText(m_hWnd, strBuffer);
}


//----------------------------------------------------------//
// EOF
//----------------------------------------------------------//
