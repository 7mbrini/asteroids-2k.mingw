/*!****************************************************************************

	@file	video.h
	@file	video.cpp

	@brief	Video manager

	@noop	author:	Francesco Settembrini
	@noop	last update: 23/6/2021
	@noop	e-mail:	mailto:francesco.settembrini@poliba.it

******************************************************************************/

#include <windows.h>
#include <wingdi.h>

#include <assert.h>


#include <locale>
#include <codecvt>
#include <string>
#include <cstring>

#include "commdefs.h"
#include "video.h"


/*!****************************************************************************
* @brief	Initialize the video system
* @param	pVideoManager Pointer to TVideoManager data structure
* @return	Returns true for success, false otherwise
******************************************************************************/
bool SetupVideoManager(TVideoManager* pVM)
{
	assert(pVM);
	assert(pVM->hWnd);
	assert(pVM->ClientArea.right);
	assert(pVM->ClientArea.bottom);


	HDC hDC = ::GetDC(pVM->hWnd);
	assert(hDC);
	
	HDC hMemDC = ::CreateCompatibleDC(hDC);
	assert(hMemDC);

	pVM->hDC = hMemDC;

	pVM->hBmp = ::CreateCompatibleBitmap(hDC,
		pVM->ClientArea.right, pVM->ClientArea.bottom);
	assert(pVM->hBmp);

	::SelectObject(pVM->hDC, pVM->hBmp);

	::ReleaseDC(pVM->hWnd, hDC);

	::SetBkMode(hMemDC, TRANSPARENT);

	return true;
}


//-----------------------------------------------------------------------------
//	SCOPO:	Clean up and quit SDL
//-----------------------------------------------------------------------------
void CleanupVideoManager(TVideoManager* pVM)
{
	assert(pVM);
	assert(pVM->hDC);
	assert(pVM->hBmp);

	::DeleteDC(pVM->hDC);
	::DeleteObject(pVM->hBmp);
}

//-----------------------------------------------------------------------------
//	SCOPO:
//-----------------------------------------------------------------------------
TVector2 GetScreenCenter(TVideoManager* pVM)
{
	return TVector2 { pVM->ClientArea.right/2.0, pVM->ClientArea.bottom/2.0 };
}

//-----------------------------------------------------------------------------
//	SCOPO:
//-----------------------------------------------------------------------------
void DrawLines(TVideoManager* pVM,
	TVecPoints& Pts, int nLineWidth, COLORREF Color, bool bClosed)
{
	assert(pVM);
	assert(pVM->hDC);

	HDC hDC = pVM->hDC;
	assert(hDC);

	HPEN hPen = ::CreatePen(PS_SOLID, nLineWidth, Color);
	assert(hPen);

	HPEN hOldPen = (HPEN) ::SelectObject(hDC, hPen);
	assert(hOldPen);

	for(int i=1; i<Pts.size(); i++)
	{
		::MoveToEx(hDC, Pts[i-1].X, Pts[i-1].Y, (LPPOINT) NULL);
		::LineTo(hDC, Pts[i].X, Pts[i].Y);
	}

	if( bClosed && (Pts.size() > 2) )
	{
		::MoveToEx(hDC, Pts[Pts.size()-1].X, Pts[Pts.size()-1].Y, (LPPOINT) NULL);
		::LineTo(hDC, Pts[0].X, Pts[0].Y);
	}

	::SelectObject(hDC, hOldPen);
	::DeleteObject(hPen);
}

//-----------------------------------------------------------------------------
//	SCOPO:
//-----------------------------------------------------------------------------
void DrawLines(TVideoManager* pVM,
	TVecVecPoints& Pts, int nLineWidth, COLORREF Color, bool bClosed)
{
	assert(pVM);

	for(int i = 0; i < Pts.size(); ++i)
	{
		DrawLines(pVM, Pts[i], nLineWidth, Color, bClosed);
	}
}

void DrawPoint(TVideoManager* pVM, TVector2& Pt, COLORREF Color)
{
	assert(pVM);
	assert(pVM->hDC);

	HDC hDC = pVM->hDC;
	assert(hDC);

	::SetPixel(hDC, Pt.X, Pt.Y, Color);
}

void ClearScreen(TVideoManager* pVM, COLORREF Color)
{
	HBRUSH hBrush = ::CreateSolidBrush(Color);
	assert(hBrush);

	HBRUSH hOldBrush = (HBRUSH) ::SelectObject(pVM->hDC, hBrush);

	::FillRect(pVM->hDC, &pVM->ClientArea, hBrush);

	::SelectObject(pVM->hDC, hOldBrush);
	::DeleteObject(hBrush);
}

bool LoadFont(TVideoManager* pVM,
	std::string strFontPath, std::wstring strName, int nSize)
{
	assert(pVM);
	assert(pVM->hDC);

	bool bResult = false;

	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	std::wstring wstrFontPath = converter.from_bytes(strFontPath);

std::string strFontName = converter.to_bytes(strName);

	//int nResults = AddFontResourceEx( wstrFontPath.c_str(), FR_PRIVATE, NULL);
	//int nResults = AddFontResourceEx( std::string(wstrFontPath).c_str(), FR_PRIVATE, NULL);
	int nResults = AddFontResourceEx( strFontPath.c_str(), FR_PRIVATE, NULL);

	if( nResults )
	{
		LOGFONT LF;
		memset(&LF, 0, sizeof(LF));

		LF.lfHeight = nSize;
		LF.lfWeight = FW_NORMAL;
		LF.lfOutPrecision = OUT_TT_ONLY_PRECIS;
		//wcscpy_s(LF.lfFaceName, strName.c_str());
		strcpy(LF.lfFaceName, strFontName.c_str());

		HFONT hFont = ::CreateFontIndirect(&LF);

		::SelectObject(pVM->hDC, hFont);

		bResult = true;
	}

	return bResult;
}

void DrawText(TVideoManager* pVM, char* pText, int nX, int nY, COLORREF nColor, UINT nAlign)
{
	assert(pText);
	assert(pVM);
	assert(pVM->hDC);

	::SetTextAlign(pVM->hDC, nAlign);
	::SetTextColor(pVM->hDC, nColor);

	::TextOutA(pVM->hDC, nX, nY, (LPCSTR) pText, strlen(pText));
}

//-----------------------------------------------------------------------------
//	SCOPO:
//-----------------------------------------------------------------------------
void DrawText(TVideoManager *pVM,
	std::vector<std::string> StringList, int nX, int nY, int nLineHeight, COLORREF nColor, UINT nAlign)
{
	assert(pVM);

	for(int i=0; i<StringList.size(); i++)
	{
		DrawText(pVM, (char*) StringList[i].c_str(), nX, nY, nColor, nAlign);

		nY += 1.25 * nLineHeight;
	}

/*
	if( StringList.size() )
	{
		for(int i=0; i<StringList.size(); ++i)
		{
			SDL_Surface *pSurface = TTF_RenderText_Solid(pGame->pFont, StringList[i].c_str(), SDL_Color { 255, 255, 255 } );
			SDL_Texture *pTexture = SDL_CreateTextureFromSurface(pGame->pSDLSystem->pRenderer, pSurface);

			int nTexW = 0, nTexH = 0;
			SDL_QueryTexture(pTexture, NULL, NULL, &nTexW, &nTexH);

			//SDL_Rect dstrect = { nX - nTexW/2.0, nY, nTexW, nTexH };	// left aligned
			SDL_Rect dstrect = { nX - nTexW/2.0, nY, nTexW, nTexH };	// centered text

			//nY += nTexH;
			nY += 1.25 * nTexH;		// adds an extra space (interline)

			SDL_RenderCopy(pGame->pSDLSystem->pRenderer, pTexture, NULL, &dstrect);

			//SDL_RenderPresent(pGame->pSDLSystem->pRenderer);

			SDL_DestroyTexture(pTexture);
			SDL_FreeSurface(pSurface);
		}
	}
*/
}

