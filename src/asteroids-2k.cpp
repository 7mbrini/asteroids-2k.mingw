/*!****************************************************************************

	@file	asteroids-2k.cpp

	@brief	Asteroids-2k main

	@noop	author:	Francesco Settembrini
	@noop	last update: 23/6/2021
	@noop	e-mail:	mailto:francesco.settembrini@poliba.it

******************************************************************************/

#include <windows.h>
#include <winuser.h>
#include <commdlg.h>

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <tchar.h>

#include "audio.h"
#include "video.h"

#include "game.h"
#include "ships.h"
#include "maths.h"
#include "utils.h"
#include "commdefs.h"

#include "resource.h"


#ifdef _MSC_VER 
	#pragma comment(linker, "/SUBSYSTEM:CONSOLE /ENTRY:WinMainCRTStartup")
#endif


//-----------------------------------------------------------------------------

#define ID_TIMER 100

//#define _DEVEL
//#define _DEBUG

//-----------------------------------------------------------------------------

HWND g_hMainWnd = 0;
HINSTANCE g_hInst = 0;
TGame* g_pGame = nullptr;

static TCHAR szTitle[] = _T(APPNAME);
static TCHAR szWindowClass[] = _T("Asteroids-2k");

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);



/*!****************************************************************************
* @brief	Setting-up the application
* @param	hWnd	Handle to main window
* @return	Returns true for success, false otherwise
******************************************************************************/
bool Setup(HWND hWnd)
{
	assert(hWnd);

	g_hMainWnd = hWnd;

	bool bResult = false;

	srand(::GetCurrentTime());

	TVideoManager *pVM = new TVideoManager();
	assert(pVM);

	pVM->hWnd = hWnd;
	pVM->ClientArea = RECT{0, 0, FRAMEW, FRAMEH };

	if( SetupVideoManager(pVM) )
	{
		TALSystem *pALSystem = new TALSystem();
		assert(pALSystem);
		
		if( SetupSoundManager(pALSystem) )
		{
			TSoundManager *pSM = new TSoundManager();
			assert(pSM);

			pSM->pALSystem = pALSystem;
			SetMasterVolume(pSM, 0.25);

			TGame* pGame = new TGame();
			assert(pGame);
			g_pGame = pGame;
			
			bResult = Setup(pGame, pVM, pSM);

#ifdef _DEVEL
			Restart(pGame);
#else
			GameOver(pGame);
#endif
		}
	}

	return bResult;
}

/*!****************************************************************************
* @brief	Cleaning-up the application
ks******************************************************************************/
int Cleanup()
{
	assert(g_pGame);
											// clean up the Game
	Cleanup(g_pGame);
											// clean up audio manager
	CleanupSoundManager(g_pGame->pSM);
											// clean up video manager
	CleanupVideoManager(g_pGame->pVM);

	return 0;
}

/*!****************************************************************************
* @noop	For debug only purpose
*
* The right syntax for windres is:
* windres resource.rc -o resource.res -F pe-x86-64 -O coff
******************************************************************************/
void KeyboardHandler(WPARAM nKey)
{
#ifdef _DEBUG

	int VK_N = 0x4E, VK_P = 0x50, VK_Q = 0x51, VK_S = 0x53, VK_X = 0x58;


	if( nKey == VK_X )
	{
		RegisterBestScore(g_pGame);
	}


/*
	if( nKey == VK_X )
	{
assert(g_hInst);
HRSRC hDlgRes = ::FindResource(g_hInst, MAKEINTRESOURCE(IDD_RECORDS), RT_DIALOG);

assert(hDlgRes);	///< @note Resource loading may not work in debug mode
if( hDlgRes ) ::MessageBeep(-1);
	}
*/

#endif
}

/*!****************************************************************************
* @brief	Keyboard handler
* @note		Uses GetAsynkKeyState() function instead of WM_KEYDOWN event
*			handler so that can handle multiple keys pressed simultaneously
******************************************************************************/
void KeyboardHandler()
{
	static SHORT nKeys[256];
	int VK_N = 0x4E, VK_P = 0x50, VK_Q = 0x51, VK_S = 0x53;

	nKeys[VK_N] = GetAsyncKeyState(0x4E);
	nKeys[VK_P] = GetAsyncKeyState(0x50);
	nKeys[VK_Q] = GetAsyncKeyState(0x51);
	nKeys[VK_S] = GetAsyncKeyState(0x53);

	nKeys[VK_LEFT] = GetAsyncKeyState(VK_LEFT);
	nKeys[VK_RIGHT] = GetAsyncKeyState(VK_RIGHT);
	nKeys[VK_UP] = GetAsyncKeyState(VK_UP);
	nKeys[VK_DOWN] = GetAsyncKeyState(VK_DOWN);
	nKeys[VK_SPACE] = GetAsyncKeyState(VK_SPACE);
	nKeys[VK_ESCAPE] = GetAsyncKeyState(VK_ESCAPE);
	nKeys[VK_ADD] = GetAsyncKeyState(VK_ADD);
	nKeys[VK_SUBTRACT] = GetAsyncKeyState(VK_SUBTRACT);

if( !IsInputDialog(g_pGame) )
{
	if( nKeys[VK_N] ) Restart(g_pGame);
	if( nKeys[VK_P] ) PauseTheGame(g_pGame);
	if( nKeys[VK_ADD] ) IncreaseMasterVolume(g_pGame->pSM);
	if( nKeys[VK_SUBTRACT] ) DecreaseMasterVolume(g_pGame->pSM);
	if( nKeys[VK_ESCAPE] | nKeys[VK_Q] ) { EndTheGame(g_pGame); PostQuitMessage(0); }

	if( IsAlive(g_pGame->pShips[scHuman]) )
	{
		if( nKeys[VK_S] ) ActivateTheShield(g_pGame->pShips[scHuman]);
		if( nKeys[VK_SPACE] ) ShotTheMissile(g_pGame, g_pGame->pShips[scHuman]);		
		if( nKeys[VK_LEFT] ) RotateLeft(g_pGame->pShips[scHuman], SHIP_ROTSTEP);
		if( nKeys[VK_RIGHT] ) RotateRight(g_pGame->pShips[scHuman], SHIP_ROTSTEP);
		if( nKeys[VK_UP] ) Impulse(g_pGame->pShips[scHuman], SHIP_IMPULSE);
	}
}
}

/*!****************************************************************************
* @brief	Force the execution speed to a specified framerate
* @param	nFPS The value of Frames per Second to be forced to
******************************************************************************/
void ForceToFPS(unsigned nFPS)
{
	assert(nFPS > 0);

	static unsigned nCurTime = ::GetTickCount();
											
	while( (::GetTickCount() - nCurTime) < 1.0/FPS * 1000.0) {;}
	nCurTime = ::GetTickCount();
}

/*!****************************************************************************
* @brief	Main application loop
******************************************************************************/
void MainLoop()
{
	assert(g_pGame);

	KeyboardHandler();

	if( IsRunning(g_pGame) && !IsPausing(g_pGame) )
	{
											// clear the screen to black
		ClearScreen(g_pGame->pVM, RGB(0,0,0));

		Run(g_pGame);
											// Force to repaint. The last paramater
											// [BOOL bErase] must be set to FALSE
											// to avoid annoying flickering effects
		InvalidateRect(g_pGame->pVM->hWnd, &g_pGame->pVM->ClientArea, FALSE);

											// Force to a specific FPS so that
											// the frame-rate is CPU independent
		ForceToFPS(FPS);
	}
}

/*!****************************************************************************
* @brief	The Win32 application entry point
* @param	hInstance	Handle to current application instance
* @param	hPrevInstance	Handle to previous application instance
* @param	lpCmdLine Pointer to the application command line string
* @param	nCmdShow Specifies how the main window must be visualized
* @note		hPrevInstance is for compatibility only, obsolete in Win32
******************************************************************************/
int WINAPI WinMain(
   _In_ HINSTANCE hInstance,
   _In_opt_ HINSTANCE hPrevInstance,
   _In_ LPSTR     lpCmdLine,
   _In_ int       nCmdShow
)
{
	WNDCLASSEX wcex;

	wcex.cbSize			= sizeof(WNDCLASSEX);
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc		= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(wcex.hInstance, IDI_APPLICATION);
	wcex.hCursor			= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm			= LoadIcon(wcex.hInstance, IDI_APPLICATION);

	if (!RegisterClassEx(&wcex))
	{
		::MessageBox(0, _T("Call to RegisterClassEx failed!"), _T(APPNAME), 0);

		return 1;
	}

	g_hInst = hInstance;

	HWND hWnd = CreateWindowEx(
		WS_EX_OVERLAPPEDWINDOW,
		szWindowClass,
		szTitle,
		WS_OVERLAPPEDWINDOW &~ (WS_THICKFRAME | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX),
		CW_USEDEFAULT, CW_USEDEFAULT,
		FRAMEW, FRAMEH,
		NULL,
		NULL,
		hInstance,
		NULL
	);

	if (!hWnd)
	{
		MessageBox(0, _T("Call to CreateWindow failed!"), _T(APPNAME), 0);

		return 1;
	}
											// set window background to black
	HBRUSH brush = CreateSolidBrush(RGB(0, 0, 0));
	SetClassLongPtr(hWnd, GCLP_HBRBACKGROUND, (LONG_PTR)brush);

											// center the window on the screen
	int nCX = ::GetSystemMetrics(SM_CXSCREEN);
	int nCY = ::GetSystemMetrics(SM_CYSCREEN);
	int nX = (nCX - FRAMEW) / 2;
	int nY = (nCY - FRAMEH) / 2;
	SetWindowPos(hWnd, 0, nX, nY, FRAMEW, FRAMEH, SWP_SHOWWINDOW);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

											// message handler
	MSG msg;

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int) msg.wParam;
}

/*!****************************************************************************
* @brief	Window Procedure: processes messages for the main window.
* @param	hWnd The handle to the window to which WndProc is referencing
* @param	message The message to be handled
* @param	wParam The wParam of the message
* @param	lParam The lParam of the message
******************************************************************************/
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

	switch (message)
	{
		case WM_CREATE:
			Setup(hWnd);
			SetTimer(hWnd, ID_TIMER, 10, NULL);
		break;

		case WM_TIMER:
			MainLoop();
		break;

		case WM_PAINT:
		{
			if( g_pGame )
			{
				PAINTSTRUCT ps;
				HDC hPaintDC = BeginPaint(hWnd, &ps);

				RECT Rect = g_pGame->pVM->ClientArea;
				::BitBlt(hPaintDC, 0, 0, Rect.right, Rect.bottom, g_pGame->pVM->hDC, 0, 0, SRCCOPY);

				EndPaint(hWnd, &ps);
			}
		}
		break;

		case WM_DESTROY:
			KillTimer(hWnd, ID_TIMER);
			Cleanup();
			PostQuitMessage(0);

		break;

		case WM_KEYDOWN:
											// see notes above on KeyboardHandler()
			KeyboardHandler(wParam);
		break;

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}

   return 0;
}
