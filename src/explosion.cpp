/**************************************************************************
 *  PipeWalker game (http://pipewalker.sourceforge.net)                   *
 *  Copyright (C) 2007-2012 by Artem Senichev <artemsen@gmail.com>        *
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
#include "mtrandom.h"
#include "render.h"
#include "level.h"


void explosion::create(const level& lvl)
{
	free();

	const unsigned short lvl_size = lvl.get_csize();
	const float scale = 10.0f / lvl_size;
	for (unsigned short y = 0; y < lvl_size; ++y) {
		const float poz_y = (-(lvl_size / 2) + 0.5f + y) * (-scale);
		for (unsigned short x = 0; x < lvl_size; ++x) {
			const float pos_x = (-(lvl_size / 2) + 0.5f + x) * scale;
			const cell& c = lvl.get_cell(x, y);
			if (c.cell_type() == cell::ct_receiver) {
				const size_t num = static_cast<size_t>(mtrandom::random(2, 5));
				for (size_t i = 0; i < num; ++i)
					_particles.push_back(create(pos_x, poz_y));
			}
		}
	}
}


explosion::particle explosion::create(const float x, const float y) const
{
	particle p;
	p.x = p.init_x = x;
	p.y = p.init_y = y;
	p.speed = mtrandom::random(-0.05f, 0.05f);
	p.life = static_cast<unsigned long>(mtrandom::random(500, 3000));
	p.stime = SDL_GetTicks();
	return p;
}


void explosion::draw(const float alpha)
{
	render& renderer = render::instance();
	for (particles::iterator it = _particles.begin(); it != _particles.end(); ++it) {
		const unsigned int diff_time = SDL_GetTicks() - it->stime;
		if (diff_time > it->life) {
			*it = create(it->init_x, it->init_y);
			continue;
		}
		const float phase = static_cast<float>(diff_time) / static_cast<float>(it->life);
		const float iphase = 1.0f - phase;
		it->y += (0.5f - phase) / 10.0f;
		it->x += it->speed * phase;
		const float scale = phase;
		const float rotate = (0.5f - phase) * 90.0f;
		const float alpha_min = iphase < alpha ? iphase : alpha;
		renderer.draw(render::exlposion, it->x, it->y, scale, alpha_min, rotate);
	}
}
