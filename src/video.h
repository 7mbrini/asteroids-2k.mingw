#ifndef _SDLSYSTEM_H_
#define _SDLSYSTEM_H_

#include <windows.h>

#include <string>

#include "vectors.h"

//#include <sdl2/sdl.h>
//#include <sdl2/sdl_audio.h>


struct TVideoManager {
	HWND hWnd;
	RECT ClientArea;

	HDC hDC;
	HBITMAP hBmp;
};

bool SetupVideoManager(TVideoManager* pVM);
void CleanupVideoManager(TVideoManager* pVM);

TVector2 GetScreenCenter(TVideoManager* pVM);
void DrawLines(TVideoManager* pVM, TVecPoints& Pts, int nLineWidth, COLORREF Color, bool bClosed=false);
void DrawLines(TVideoManager* pVM, TVecVecPoints& Pts, int nLineWidth, COLORREF Color, bool bClosed=false);

void DrawPoint(TVideoManager* pVM, TVector2& Pt, COLORREF Color);

void ClearScreen(TVideoManager* pVM, COLORREF Color);

bool LoadFont(TVideoManager* pVM, std::string strFontPath, std::wstring strName, int nSize);

void DrawText(TVideoManager* pVM, char* pText, int nX, int nY, COLORREF nColor = RGB(255,255,255), UINT nAlign = TA_CENTER);
void DrawText(TVideoManager* pVM, std::vector<std::string> StringList, int nX, int nY,
	int nTextH, COLORREF nColor = RGB(255,255,255), UINT nAlign = TA_CENTER);

#endif


