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

#include "button.h"


CButton::CButton()
:	m_X(0.0f),
	m_Y(0.0f),
	m_Width(0.0f),
	m_Height(0.0f),
	m_TexId(0),
	m_BtnId(0)
{
}


CButton::CButton(const float x, const float y, const float width, const float height, const GLuint tex, const int id)
:	m_X(x),
	m_Y(y),
	m_Width(width),
	m_Height(height),
	m_TexId(tex),
	m_BtnId(id)
{
}


bool CButton::IsMouseOver(const float x, const float y) const
{
	return  x >= m_X && x <= m_X + m_Width &&
			y <= m_Y && y >= m_Y - m_Height;
}


void CButton::Render(const float x, const float y) const
{
	RenderButton(x, y, m_TexId);
}


void CButton::RenderButton(const float x, const float y, const GLuint texture) const
{
	const float coeff = IsMouseOver(x, y) ? 0.05f : 0.0f;

	//Button vertex coordinates
	const float Vertex[] = {
		m_X - coeff, m_Y + coeff,
		m_X - coeff, m_Y - m_Height - coeff,
		m_X + m_Width + coeff, m_Y - m_Height - coeff,
		m_X + m_Width + coeff, m_Y + coeff };

	static const short Tex[] =			{ 0, 1, 0, 0, 1, 0, 1, 1 };	//Texture coordinates
	static const unsigned int Ind[] =	{ 0, 1, 2, 0, 2, 3 };		//Indices

	glBindTexture(GL_TEXTURE_2D, texture);
	glVertexPointer(2, GL_FLOAT, 0, Vertex);
	glTexCoordPointer(2, GL_SHORT, 0, Tex);
	glDrawElements(GL_TRIANGLES, (sizeof(Ind) / sizeof(Ind[0])), GL_UNSIGNED_INT, Ind);
}


CRadioButton::CRadioButton(const bool state, const float x, const float y, const float width, const float height, const GLuint texOn, const GLuint texOff, const int id)
: CButton(x, y, width, height, texOn, id), m_State(state), m_TexOff(texOff)
{
}


void CRadioButton::Render(const float x, const float y) const
{
	RenderButton(x, y, m_State ? m_TexId : m_TexOff);
}
