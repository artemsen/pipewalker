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

#include "mode_puzzle.h"
#include "sound.h"
#include "mtrandom.h"
#include "settings.h"

#define PW_SWAP_MAP_SPEED 500


mode_puzzle::mode_puzzle()
:	_btn_reset(render::btn_reset, render::btn_reset_s, PW_BUTTON_X1, PW_BUTTON_Y),
	_btn_prev(render::btn_prev,   render::btn_prev_s,  PW_BUTTON_X2, PW_BUTTON_Y),
	_btn_next(render::btn_next,   render::btn_next_s,  PW_BUTTON_X3, PW_BUTTON_Y),
	_btn_sett(render::btn_sett,   render::btn_sett_s,  PW_BUTTON_X4, PW_BUTTON_Y),
	_trans_stime(0),
	_curr_level(NULL), _next_level(NULL)
{
}


bool mode_puzzle::draw(const float alpha)
{
	bool redisplay = false;

	float alpha_min = alpha;

	render::instance().draw_cell_field(_curr_level->get_csize(), alpha);

	if (_trans_stime != 0) {
		//Map changing
		redisplay = true;
		const unsigned int diff_time = SDL_GetTicks() - _trans_stime;
		if (diff_time > PW_SWAP_MAP_SPEED) {
			//Transition complete
			_trans_stime = 0;
			_win_anim.free(); //Not need anymore

			level* swap_tmp = _curr_level;
			_curr_level = _next_level;
			_next_level = swap_tmp;
		}
		else {
			assert(_next_level);
			alpha_min = static_cast<float>(diff_time) / static_cast<float>(PW_SWAP_MAP_SPEED);
			draw_level(*_next_level, alpha_min);
			alpha_min = 1.0f - alpha_min;	//Invert
		}
	}

	const cell::state rs = _curr_level->calculate_state();
	draw_level(*_curr_level, alpha_min);
	if (rs == cell::st_rcomplete) {
		if (!_curr_level->solved())
			sound::play(sound::clatz);
		else {
			sound::play(sound::complete);
			_win_anim.create(*_curr_level); //Prepare winner animation
		}
	}
	if (rs != cell::st_unchanged)
		redisplay = true;

	//Draw winner animation
	if (_curr_level->solved()) {
		_win_anim.draw(alpha_min);
		redisplay = true;
	}

	//Draw buttons
	redisplay |= _btn_reset.draw(alpha);
	redisplay |= _btn_prev.draw(alpha);
	redisplay |= _btn_next.draw(alpha);
	redisplay |= _btn_sett.draw(alpha);

	return redisplay;
}


bool mode_puzzle::on_mouse_move(const float x, const float y)
{
	bool redisplay = false;

	redisplay |= _btn_reset.on_mouse_move(x, y);
	redisplay |= _btn_prev.on_mouse_move(x, y);
	redisplay |= _btn_next.on_mouse_move(x, y);
	redisplay |= _btn_sett.on_mouse_move(x, y);

	return redisplay;
}


bool mode_puzzle::on_mouse_click(const float x, const float y, const Uint8 btn)
{
	if (_trans_stime != 0)
		return false;

	bool mode_finished = false;

	if (!_curr_level->solved() && x > -5.0f && x < 5.0f && y > -5.0f && y < 5.0f) {
		//Cursor inside game field area
		const float scale = 10.0f / static_cast<float>(_curr_level->get_csize());
		const unsigned short xc = static_cast<unsigned short>((x + 5.0f) / scale);
		const unsigned short yc = _curr_level->get_csize() - 1 - static_cast<unsigned short>((y + 5.0f) / scale);
		if ((btn & SDL_BUTTON(SDL_BUTTON_LEFT)) || (btn & SDL_BUTTON(SDL_BUTTON_RIGHT)))
			_curr_level->rotate(xc, yc, (btn & SDL_BUTTON(SDL_BUTTON_RIGHT)) != 0);
		else if (btn & SDL_BUTTON(SDL_BUTTON_MIDDLE))
			_curr_level->reverse_lock(xc, yc);
	}
	else if (btn & SDL_BUTTON(SDL_BUTTON_LEFT)) {
		//Che for buttons click
		if (_btn_reset.cross(x, y)) {
			_win_anim.free();
			_curr_level->rotate_all();
		}
		else if (_btn_sett.cross(x, y))
			mode_finished = true;
		else {
			const bool prev_btn = _btn_prev.cross(x, y);
			const bool next_btn = _btn_next.cross(x, y);
			if (prev_btn || next_btn) {
				unsigned long new_map_id = _curr_level->id();
				if (!settings::rndlvl_mode())
					new_map_id += (prev_btn ? -1 : 1);
				else {
					mtrandom::seed(SDL_GetTicks());
					new_map_id = static_cast<unsigned long>(mtrandom::random(1, PW_MAX_LEVEL_NUMBER));
				}
				if (new_map_id != 0 && new_map_id < PW_MAX_LEVEL_NUMBER)
					switch_to_level(new_map_id, _curr_level->get_tsize(), _curr_level->wrap_mode());
			}
		}
	}

	return mode_finished;
}


void mode_puzzle::initialize()
{
	_curr_level = &_levels[0];
	_next_level = &_levels[1];

	//Load last saved level
	unsigned long id;
	string state;
	const level::size sz = settings::last_size();
	const bool wm = settings::last_wrap();
	settings::get_state(sz, wm, id, state);
	if (!_curr_level->load(id, sz, wm, state))
		_curr_level->create(id, sz, wm);
}


void mode_puzzle::on_settings_changed(const level::size sz, const bool wrap)
{
	if (sz != _curr_level->get_tsize() || wrap != _curr_level->wrap_mode())
		switch_to_level(0, sz, wrap);
}


void mode_puzzle::save_current_level() const
{
	settings::set_state(
		_curr_level->id(),
		_curr_level->get_tsize(),
		_curr_level->wrap_mode(),
		_curr_level->save());
}


void mode_puzzle::draw_level(const level& lvl, const float alpha) const
{
	const unsigned short sz = lvl.get_csize();

	render& renderer = render::instance();
	renderer.draw_text(-2.2f, -5.25f, 0.7f, alpha, lvl.id_text());

	const float scale = 10.0f / static_cast<float>(_curr_level->get_csize());
	for (unsigned short y = 0; y < sz; ++y) {
		const float poz_y = (-(sz / 2) + 0.5f + y) * (-scale);
		for (unsigned short x = 0; x < sz; ++x) {
			const float pos_x = (-(sz / 2) + 0.5f + x) * scale;
			const cell& curr_cell = lvl.get_cell(x, y);
			const bool cell_active = curr_cell.active();
			const cell::ctype cell_type = curr_cell.cell_type();

			//Draw tube
			if (cell_type != cell::ct_free) {
				render::img_type tex_id = render::cell_bkgr;
				//Draw tube shadow
				switch (curr_cell.tube_type()) {
					case cell::tt_half:     tex_id = render::tube_hs; break;
					case cell::tt_straight: tex_id = render::tube_ss; break;
					case cell::tt_curved:   tex_id = render::tube_cs; break;
					case cell::tt_joiner:   tex_id = render::tube_js; break;
					default:
						assert(false && "Unknown tube type");
						break;
				}
				renderer.draw(tex_id, pos_x + 0.03f, poz_y - 0.03f, scale, alpha, curr_cell.angle());

				//Draw tube
				switch (curr_cell.tube_type()) {
					case cell::tt_half:
						tex_id = (cell_active ? render::tube_ha : render::tube_hp);
						break;
					case cell::tt_straight:
						tex_id = (cell_active ? render::tube_sa : render::tube_sp);
						break;
					case cell::tt_curved:
						tex_id = (cell_active ? render::tube_ca : render::tube_cp);
						break;
					case cell::tt_joiner:
						tex_id = (cell_active ? render::tube_ja : render::tube_jp);
						break;
					default:
						assert(false && "Unknown tube type");
						break;
				}
				renderer.draw(tex_id, pos_x, poz_y, scale, alpha, curr_cell.angle());
			}

			//Draw objects
			if (cell_type == cell::ct_sender)
				renderer.draw(render::sender, pos_x, poz_y, scale, alpha);
			else if (cell_type == cell::ct_receiver)
				renderer.draw(cell_active ? render::rcvr_act : render::rcvr_psv, pos_x, poz_y, scale, alpha);

			//Draw lock
			if (curr_cell.locked())
				renderer.draw(render::lock, pos_x, poz_y, scale, alpha);
		}
	}
}


void mode_puzzle::switch_to_level(const unsigned long id, const level::size sz, const bool wrap_mode)
{
	_win_anim.free();

	if (_curr_level->get_tsize() == sz && _curr_level->wrap_mode() == wrap_mode) {
		assert(id);
		_next_level->create(id, sz, wrap_mode);
		_trans_stime = SDL_GetTicks();	//Start level changing
	}
	else {
		save_current_level();

		//Try to load last saved state
		unsigned long id;
		string state;
		settings::get_state(sz, wrap_mode, id, state);
		if (!_curr_level->load(id, sz, wrap_mode, state))
			_curr_level->create(id, sz, wrap_mode);
	}
}
