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

#include "rendertext.h"
#include "image.h"

#define FONT_INT_TEXTURE_FILENAME	"font.tga"
#define CHARACTERS_NUM				96
#define TEXT_TEX_COL				16
#define TEXT_TEX_ROW				8

CTexture CRenderText::m_Texture;


void CRenderText::Free(void)
{
	m_Texture.Free();
}


void CRenderText::Load()
{
	CRenderText::Free();
	//Load font texture
	m_Texture.Load(DIR_GAMEDATA FONT_INT_TEXTURE_FILENAME);
}


void CRenderText::Print(const float x, const float y, const float scale, const float color[4], const bool shadow, const char* text)
{
	assert(text);

	//Draw fake shadow
	const float shadowColor[4] = { 0.1f, 0.1f, 0.1f, color[3] };
	if (shadow)
		Print(x + scale / 10.0f, y - scale / 10.0f, scale, shadowColor, false, text);

	static const float symbolWidth =  1.0f / TEXT_TEX_COL;
	static const float symbolHeight = 1.0f / TEXT_TEX_ROW;

	static const float textVertex[] =			{ 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, -1.0f, 1.0f, 0.0f };
	static const unsigned short textIndices[] =	{ 0, 1, 2, 0, 2, 3 };

	glColor4f(color[0], color[1], color[2], color[3]);
	glDisable(GL_DEPTH_TEST);
	glPushMatrix();
		glTranslatef(x, y, 0);
		glScalef(scale, scale, scale);
		glBindTexture(GL_TEXTURE_2D, m_Texture.GetId());
		glVertexPointer(2, GL_FLOAT, 0, textVertex);
	
		while (*text) {
			unsigned char row = 1;
			unsigned char col = static_cast<unsigned char>(*text - ' ');
			while (col >= TEXT_TEX_COL) {
				++row;
				col -= TEXT_TEX_COL;
			}
			const float baseTexX = symbolWidth * static_cast<float>(col);
			const float baseTexY = 1.0f - symbolHeight * static_cast<float>(row);

 			const float textTexture[] = {
 				baseTexX, baseTexY + symbolHeight,
 				baseTexX, baseTexY,
 				baseTexX + symbolWidth, baseTexY,
 				baseTexX + symbolWidth, baseTexY + symbolHeight
 			};

			glTexCoordPointer(2, GL_FLOAT, 0, textTexture);
			glDrawElements(GL_TRIANGLES, (sizeof(textIndices) / sizeof(textIndices[0])), GL_UNSIGNED_SHORT, textIndices);

			++text;
			glTranslatef(1.0f + scale / 20.0f, 0.0f, 0.0f);
		}

	glPopMatrix();
	glEnable(GL_DEPTH_TEST);
	glColor4f(1.0f, 1.0, 1.0f, 0.0f);
}
