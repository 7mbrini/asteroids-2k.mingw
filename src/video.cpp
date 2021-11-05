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
* @param	pVM Pointer to TVideoManager data structure
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


/*!****************************************************************************
* @brief	Cleanup the video system
* @param	pVM Pointer to TVideoManager data structure
******************************************************************************/
void CleanupVideoManager(TVideoManager* pVM)
{
	assert(pVM);
	assert(pVM->hDC);
	assert(pVM->hBmp);

	::DeleteDC(pVM->hDC);
	::DeleteObject(pVM->hBmp);
}

/*!****************************************************************************
* @brief	Gets client area coordinates
* @param	pVM Pointer to TVideoManager data structure
* @return	Returns the center coordinate of the client area
******************************************************************************/
TVector2 GetScreenCenter(TVideoManager* pVM)
{
	return TVector2 { pVM->ClientArea.right/2.0, pVM->ClientArea.bottom/2.0 };
}

/*!****************************************************************************
* @brief	Draws ines
* @param	pVM Pointer to TVideoManager data structure
* @param	Pts Vector of line vertices
* @param	nLineWidth Thickness of the line to be drawn
* @param	Color Color of the polyline to be drawn
* @param	bClosed Flag for closing: true for closed polylines
******************************************************************************/
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

/*!****************************************************************************
* @brief	Draws a series of polylines
* @param	pVM Pointer to TVideoManager data structure
* @param	Pts Vector of polylines
* @param	nLineWidth Thickness of the polyline to be drawn
* @param	Color Color of the polyline to be drawn
* @param	bClosed Flag for closing: true for closed polylines
******************************************************************************/
void DrawLines(TVideoManager* pVM,
	TVecVecPoints& Pts, int nLineWidth, COLORREF Color, bool bClosed)
{
	assert(pVM);

	for(int i = 0; i < Pts.size(); ++i)
	{
		DrawLines(pVM, Pts[i], nLineWidth, Color, bClosed);
	}
}

/*!****************************************************************************
* @brief	Draws a point
* @param	pVM Pointer to TVideoManager data structure
* @param	Pt Coordinates of point to be drawn
* @param	Color Color of the polyline to be drawn
******************************************************************************/
void DrawPoint(TVideoManager* pVM, TVector2& Pt, COLORREF Color)
{
	assert(pVM);
	assert(pVM->hDC);

	HDC hDC = pVM->hDC;
	assert(hDC);

	::SetPixel(hDC, Pt.X, Pt.Y, Color);
}

/*!****************************************************************************
* @brief	Clears the screen
* @param	pVM Pointer to TVideoManager data structure
* @param	Color The filling color
******************************************************************************/
void ClearScreen(TVideoManager* pVM, COLORREF Color)
{
	HBRUSH hBrush = ::CreateSolidBrush(Color);
	assert(hBrush);

	HBRUSH hOldBrush = (HBRUSH) ::SelectObject(pVM->hDC, hBrush);

	::FillRect(pVM->hDC, &pVM->ClientArea, hBrush);

	::SelectObject(pVM->hDC, hOldBrush);
	::DeleteObject(hBrush);
}

/*!****************************************************************************
* @brief	Loads a true type (ttf) font from file
* @param	pVM Pointer to TVideoManager data structure
* @param	strFontPath Full path to file containing the font
* @param	strName Name for the font
* @param	nSize The size of the font
******************************************************************************/
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

/*!****************************************************************************
* @brief	Draws a text
* @param	pVM Pointer to TVideoManager data structure
* @param	pText Pointer to a text string
* @param	nX The X coordinate for the text
* @param	nY The Y coordinate for the text
* @param	nColor The color for the text
* @param	nAlign The alignment for the text
******************************************************************************/
void DrawText(TVideoManager* pVM, char* pText, int nX, int nY, COLORREF nColor, UINT nAlign)
{
	assert(pText);
	assert(pVM);
	assert(pVM->hDC);

	::SetTextAlign(pVM->hDC, nAlign);
	::SetTextColor(pVM->hDC, nColor);

	::TextOutA(pVM->hDC, nX, nY, (LPCSTR) pText, strlen(pText));
}

/*!****************************************************************************
* @brief	Draws a multiple lines of text
* @param	pVM Pointer to TVideoManager data structure
* @param	StringList A list of text strings
* @param	nX The X coordinate for the text
* @param	nY The Y coordinate for the text
* @param	nLineHeight The height for the text
* @param	nColor The color for the text
* @param	nAlign The alignment for the text
******************************************************************************/
void DrawText(TVideoManager *pVM,
	std::vector<std::string> StringList, int nX, int nY, int nLineHeight, COLORREF nColor, UINT nAlign)
{
	assert(pVM);

	for(int i=0; i<StringList.size(); i++)
	{
		DrawText(pVM, (char*) StringList[i].c_str(), nX, nY, nColor, nAlign);

		nY += 1.25 * nLineHeight;
	}
}

