/**************************************************************************
 *  PipeWalker game (http://pipewalker.sourceforge.net)                   *
 *  Copyright (C) 2007-2010 by Artem A. Senichev <artemsen@gmail.com>     *
 *                                                                        *
 *  This program is free software: you can redistribute it and/or modify  *
 *  it under the terms of the GNU General Public License as published by  *
 *  the Free Software Foundation, either version 3 of the License, or     *
 *  (at your option) any later version.                                   *
 *                                                                        *
 *  This program is distributed in the hope that it will be useful,       *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *  GNU General Public License for more details.                          *
 *                                                                        *
 *  You should have received a copy of the GNU General Public License     *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 **************************************************************************/

#pragma once

#include "common.h"
#include "texture.h"


class CRenderText
{
public:
	/**
	 * Load font data
	 */
	static void Load();

	/**
	 * Free font data
	 */
	static void Free();

	/**
	 * Print string
	 * \param type font type
	 * \param x an X coordinate of top left text corner (window coordinates begin from top left)
	 * \param y an Y coordinate of top left text corner (window coordinates begin from top left)
	 * \param scale scale coefficient
	 * \param color[4] pointer to RGBA text color value
	 * \param shadow draw shadow flag
	 * \param text output string
	 */
	static void Print(const float x, const float y, const float scale, const float color[4], const bool shadow, const char* text);


private:	//Class variables
	static CTexture	m_Texture;		///< Fonts texture
};



/////////////////////////////////////////////////////////////
// This code I used to export font to bitmap in MS Windows //
/////////////////////////////////////////////////////////////
#if 0	//Never compile it, reserved for future use only (update font file for example :-))
void ImportFont()
{
	const int imageWidth = 512;
	const int imageHeight = 256;
	const int imageCol = 16;
	const int imageStr = 6;

	const int fontSize = 45;
	const wchar_t* fontName = L"Consolas";
	const wchar_t* fontFile = L"D:\\outFont.bmp";

	unsigned char firstSymbol = 32;	//' '

	HDC hDC = CreateCompatibleDC(NULL);
	if (!hDC)
		return;

	BITMAPINFO bmpInfo;
	memset(&bmpInfo, 0, sizeof(BITMAPINFO));
	bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmpInfo.bmiHeader.biWidth = imageWidth;
	bmpInfo.bmiHeader.biHeight = imageHeight;
	bmpInfo.bmiHeader.biBitCount = 24;
	bmpInfo.bmiHeader.biPlanes = 1;

	void* bmpData = NULL;
	HBITMAP hBitmap = CreateDIBSection(hDC, &bmpInfo, DIB_RGB_COLORS, &bmpData, NULL, 0);
	if (!hBitmap)
		return;

	HFONT hFont = CreateFontW(fontSize, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY /*ANTIALIASED_QUALITY*/, DEFAULT_PITCH, fontName);
	if (!hFont)
		return;

	SelectObject(hDC, hBitmap);
	SelectObject(hDC, hFont);
	SetBkColor(hDC, RGB(0, 0, 0));
	SetTextColor(hDC, RGB(255, 255, 255));

	const int cellWidth = imageWidth / imageCol;
	const int cellHeight = imageHeight / imageStr;
	int x = 0;
	int y = 0;
	while (firstSymbol < 128) {
		RECT rc;
		rc.left   = cellWidth * x++;
		rc.right  = rc.left + cellWidth;
		rc.top    = cellHeight * y;
		rc.bottom = rc.top + cellHeight;

		if (x >= imageCol) {
			x = 0;
			++y;
		}

		char ansi[2] = { static_cast<char>(firstSymbol), 0 };
		wchar_t wide[2];
		MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, ansi, 1, wide, 2);
		DrawTextW(hDC, wide, 1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

		++firstSymbol;
	}

	//Write BMP file
	PICTDESC picDesc;
	picDesc.cbSizeofstruct = sizeof(PICTDESC);
	picDesc.picType = PICTYPE_BITMAP;
	picDesc.bmp.hbitmap = hBitmap;
	picDesc.bmp.hpal = NULL;
	IDispatch* iPic;
	OleCreatePictureIndirect(&picDesc, IID_IDispatch, FALSE, (LPVOID*)&iPic);
	OleSavePictureFile(iPic, CComBSTR(fontFile));
	iPic->Release();
	
	//Cleanup
	DeleteObject(hFont);
	DeleteObject(hBitmap);
	DeleteDC(hDC);
}
#endif
