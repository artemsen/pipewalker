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

#include "explosion.h"
#include "texture.h"


CExplosion::CExplosion(const float x, const float y)
	: m_X(x), m_Y(y), m_Force(0.0f)
{
	Renew();
}


void CExplosion::Render()
{
	glDisable(GL_DEPTH_TEST);

	bool explosionActive = false;

	const size_t sz = m_Particles.size();
	for (size_t i = 0; i < sz; ++i) {
		if (m_Particles[i].Life > 0.0f) {

			explosionActive = true;

			const float radius = (m_Force / 2.0f) * (1.0f - m_Particles[i].Life);

			glColor4f(m_Particles[i].Life + 0.5f, 1.0f - m_Particles[i].Life + 0.5f, 0.0f, m_Particles[i].Life > 0.1f ? m_Particles[i].Life : 0.1f);

 			const float vertex[] = {
				 m_Particles[i].PosX - radius, m_Particles[i].PosY + radius,
				 m_Particles[i].PosX - radius, m_Particles[i].PosY - radius,
				 m_Particles[i].PosX + radius, m_Particles[i].PosY - radius,
				 m_Particles[i].PosX + radius, m_Particles[i].PosY + radius
			};
 			static const short texture[] =			{ 0, 1, 0, 0, 1, 0, 1, 1 };
 			static const unsigned int indices[] =	{ 0, 1, 2, 0, 2, 3 };

  			glBindTexture(GL_TEXTURE_2D, CTextureBank::Get(CTextureBank::TexExplosionPart));
 			glVertexPointer(2, GL_FLOAT, 0, vertex);
 			glTexCoordPointer(2, GL_SHORT, 0, texture);
 			glDrawElements(GL_TRIANGLES, (sizeof(indices) / sizeof(indices[0])), GL_UNSIGNED_INT, indices);

			m_Particles[i].PosX += m_Particles[i].SpeedX;
			m_Particles[i].PosY += m_Particles[i].SpeedY;
			m_Particles[i].Life -= m_Particles[i].FadeSpeed;
		}
	}

	glColor3f(1.0f, 1.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	if (!explosionActive)
		Renew();
}


void CExplosion::Renew()
{
	m_Force = 0.5f + static_cast<float>(rand() % 50) / 100.0f;

	const size_t num = static_cast<size_t>(m_Force * 10.0f);
	m_Particles.resize(num);

	for (size_t i = 0; i < num; ++i) {
		//Initialize particle
		m_Particles[i].Life = m_Force;
		m_Particles[i].FadeSpeed = static_cast<float>(rand() % 100) / 1000.0f + 0.005f;
		m_Particles[i].PosX = m_X;
		m_Particles[i].PosY = m_Y;
		m_Particles[i].SpeedX = 0.05f - static_cast<float>(rand() % 100) / 1000.0f;
		m_Particles[i].SpeedY = 0.05f - static_cast<float>(rand() % 100) / 1000.0f;
	}
}