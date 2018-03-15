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

#include "explosion.h"
#include "texture.h"
#include "mtrandom.h"
#include "game.h"


CExplosion::CExplosion(CGame& game, const float x, const float y)
	: _X(x), _Y(y), _Force(0.0f), _Game(game)
{
	Renew();
}


void CExplosion::Render()
{
	glDisable(GL_DEPTH_TEST);

	bool explosionActive = false;

	static const float vertex[] =			{ -0.2f, 0.2f, -0.2f, -0.2f, 0.2f, -0.2f, 0.2f, 0.2f };
	static const short texture[] =			{ 0, 1, 0, 0, 1, 0, 1, 1 };
	static const unsigned short indices[] =	{ 0, 1, 2, 0, 2, 3 };
	
	glBindTexture(GL_TEXTURE_2D, _Game.TextureBank().Get(CTextureBank::TexExplosionPart));
	glVertexPointer(2, GL_FLOAT, 0, vertex);
	glTexCoordPointer(2, GL_SHORT, 0, texture);

	const size_t sz = _Particles.size();
	for (size_t i = 0; i < sz; ++i) {
		if (_Particles[i].Life > 0.0f) {

			explosionActive = true;

			const float radius = 3.0f - _Particles[i].Life * 3.0f;

			const float rotateAngle = _Particles[i].Life * 180.0f;

			glColor4f(1.0f, 1.0f, 1.0f, _Particles[i].Life > 0.1f ? _Particles[i].Life : 0.1f);
			glPushMatrix();
				glTranslatef(_Particles[i].PosX, _Particles[i].PosY, 0.0f);
				glRotatef(rotateAngle, 0.0f, 0.0f, 1.0f);
				glScalef(radius, radius, 0.0f);
				glDrawElements(GL_TRIANGLES, (sizeof(indices) / sizeof(indices[0])), GL_UNSIGNED_SHORT, indices);
			glPopMatrix();

			_Particles[i].SpeedX -= _Particles[i].FadeSpeed / 100.0f;
			_Particles[i].SpeedY -= _Particles[i].FadeSpeed / 100.0f;

			_Particles[i].PosX += _Particles[i].SpeedX;
			_Particles[i].PosY += _Particles[i].SpeedY;
			_Particles[i].Life -= _Particles[i].FadeSpeed;
		}
	}

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	if (!explosionActive)
		Renew();
}


void CExplosion::Renew()
{
	_Force = 0.5f + static_cast<float>(CMTRandom::Rand() % 50) / 100.0f;

	const size_t num = static_cast<size_t>(_Force * 10.0f);
	_Particles.resize(num);

	for (size_t i = 0; i < num; ++i) {
		//Initialize particle
		_Particles[i].Life = _Force;
		_Particles[i].FadeSpeed = static_cast<float>(CMTRandom::Rand() % 100) / 1000.0f + 0.005f;
		_Particles[i].PosX = _X;
		_Particles[i].PosY = _Y;
		_Particles[i].SpeedX = 0.05f - static_cast<float>(CMTRandom::Rand() % 100) / 1000.0f;
		_Particles[i].SpeedY = 0.05f - static_cast<float>(CMTRandom::Rand() % 100) / 1000.0f;
	}
}
