/**************************************************************************
 *  PipeWalker game (http://pipewalker.sourceforge.net)                   *
 *  Copyright (C) 2007-2009 by Artem A. Senichev <artemsen@gmail.com>     *
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
#define FONT_BASE_SIZE				0.5f

#define CHARACTERS_NUM				96


GLuint CRenderText::m_DispList = 0;
CTexture CRenderText::m_Texture;


void CRenderText::Free(void)
{
	if (m_DispList)
		glDeleteLists(m_DispList, CHARACTERS_NUM);
	m_DispList = 0;

	m_Texture.Free();
}


void CRenderText::Load()
{
	CRenderText::Free();

	//Load font texture
	m_Texture.Load(DIR_GAMEDATA FONT_INT_TEXTURE_FILENAME);

	//Create display lists with font characters
	m_DispList = glGenLists(CHARACTERS_NUM);
	const float normTexCol = 16;
	const float normTexRow = 8;
	for (unsigned int i = 0; i < CHARACTERS_NUM; ++i) {
		const float cx = static_cast<float>(i % static_cast<int>(normTexCol)) / normTexCol;
		const float cy = static_cast<float>(i / static_cast<int>(normTexCol)) / normTexRow;
		glNewList(m_DispList + i, GL_COMPILE);
			glBindTexture(GL_TEXTURE_2D, m_Texture.GetId());
			glBegin(GL_TRIANGLE_STRIP);
				glTexCoord2f(cx, 1.0f - cy); glVertex2i(0, 0);
				glTexCoord2f(cx, 1.0f - cy - 1.0f / normTexRow); glVertex2f(0.0f, -FONT_BASE_SIZE);
				glTexCoord2f(cx + 1.0f / normTexCol, 1.0f - cy); glVertex2f(FONT_BASE_SIZE, 0.0f);
				glTexCoord2f(cx + 1.0f / normTexCol, 1.0f - cy - 1.0f / normTexRow); glVertex2f(FONT_BASE_SIZE, -FONT_BASE_SIZE);
			glEnd();
			glTranslatef(FONT_BASE_SIZE - FONT_BASE_SIZE / 3.0f, 0.0f, 0.0f);
		glEndList();
	}
}


void CRenderText::Print(const float x, const float y, const float scale, const float color[4], const bool shadow, const char* text)
{
	assert(text);

	//Draw fake shadow
	const float shadowColor[4] = { 0.0f, 0.0f, 0.0f, color[3] };
	if (shadow)
		Print(x + scale / 30.0f, y - scale / 30.0f, scale, shadowColor, false, text);

	glColor4fv(color);
	glDisable(GL_DEPTH_TEST);
	glPushMatrix();
		glTranslatef(x, y, 0);
		glScalef(scale, scale, scale);
		glListBase(m_DispList - 32);
		glCallLists(strlen(text), GL_UNSIGNED_BYTE, text);
	glPopMatrix();
	glEnable(GL_DEPTH_TEST);
	glColor4f(1.0f, 1.0, 1.0f, 0.0f);
}
