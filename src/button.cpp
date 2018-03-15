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
:	m_x(0.0f),
	m_y(0.0f),
	m_width(0.0f),
	m_height(0.0f),
	m_tex(0),
	m_id(0)
{
}


void CButton::Create(const float x, const float y, const float witdh, const float height, const GLuint tex, const int id)
{
	m_x = x;
	m_y = y;
	m_width = witdh;
	m_height = height;
	m_tex = tex;
	m_id = id;
}


bool CButton::IsMouseOver(const float x, const float y) const
{
	return  x >= m_x && x <= m_x + m_width &&
			y <= m_y && y >= m_y - m_height;
}


void CButton::Render(const float x, const float y) const
{
	const float coeff = IsMouseOver(x, y) ? 0.05f : 0.0f;

	//Button vertex coordinates
	const float Vertex[] = {
		m_x - coeff, m_y + coeff,
		m_x - coeff, m_y - m_height - coeff,
		m_x + m_width + coeff, m_y - m_height - coeff,
		m_x + m_width + coeff, m_y + coeff };

	static const short Tex[] =			{ 0, 1, 0, 0, 1, 0, 1, 1 };	//Texture coordinates
	static const unsigned int Ind[] =	{ 0, 1, 2, 0, 2, 3 };		//Indices

	glBindTexture(GL_TEXTURE_2D, m_tex);
	glVertexPointer(2, GL_FLOAT, 0, Vertex);
	glTexCoordPointer(2, GL_SHORT, 0, Tex);
	glDrawElements(GL_TRIANGLES, (sizeof(Ind) / sizeof(Ind[0])), GL_UNSIGNED_INT, Ind);
}
