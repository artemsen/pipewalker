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

#include "mode_settings.h"
#include "render.h"
#include "settings.h"
#include "game.h"


mode_settings::mode_settings()
:	_btn_ok(render::btn_ok, render::btn_ok_s, PW_BUTTON_X1, PW_BUTTON_Y),
	_btn_cancel(render::btn_cancel, render::btn_cancel_s, PW_BUTTON_X4, PW_BUTTON_Y),
	_wrap_mode (true, -0.6f, -0.3f, 0.7f),
	_rnd_mode(false,  -0.6f, -1.0f, 0.7f),
	_sound_mode(true, -0.6f, -2.3f, 0.7f),
	_prev_theme(render::btn_prev, render::btn_prev_s, -2.3f, -3.5f, 0.7f),
	_next_theme(render::btn_next, render::btn_next_s,  2.3f, -3.5f, 0.7f),
	_sett_size(level::sz_normal),
	_sett_wrap(true),
	_sett_rnd(false),
	_sett_sound(true)
{
	_map_size[0].setup(false, -0.6f, 2.9f, 0.7f, level::sz_small);
	_map_size[1].setup(false, -0.6f, 2.2f, 0.7f, level::sz_normal);
	_map_size[2].setup(false, -0.6f, 1.5f, 0.7f, level::sz_big);
	_map_size[3].setup(false, -0.6f, 0.8f, 0.7f, level::sz_extra);
}


bool mode_settings::draw(const float alpha)
{
	const char* theme_name = settings::theme();
	assert(theme_name);

	render& renderer = render::instance();
	renderer.draw_text(-2.0f,  5.0f, 0.4f, alpha, "Version " PACKAGE_VERSION);
	renderer.draw_text(-4.6f,  4.5f, 0.9f, alpha, "Game settings");

	renderer.draw_text(-3.6f,  3.2f, 0.7f, alpha, "Map:");
	renderer.draw_text( 0.0f,  3.2f, 0.7f, alpha, "Small");
	renderer.draw_text( 0.0f,  2.5f, 0.7f, alpha, "Normal");
	renderer.draw_text( 0.0f,  1.8f, 0.7f, alpha, "Big");
	renderer.draw_text( 0.0f,  1.1f, 0.7f, alpha, "Extra");

	renderer.draw_text( 0.0f,  0.0f, 0.7f, alpha, "Wrap mode");
	renderer.draw_text( 0.0f, -0.7f, 0.7f, alpha, "Random");

	renderer.draw_text(-4.6f, -2.0f, 0.7f, alpha, "Sound:");

	renderer.draw_text(-1.4f, -3.2f, 0.7f, alpha, "Theme");
	renderer.draw_text(0.0f - ((static_cast<float>(strlen(theme_name)) / 2.0f) * 0.55f), -4.0f, 0.7f, alpha, theme_name);

	renderer.draw_text(-3.65f, -5.2f, 0.35f, alpha, "(C) 2007-2012 Artem Senichev");
	renderer.draw_text(-1.90f, -5.6f, 0.35f, alpha, "Moscow, Russia");

	bool redisplay = false;

	//Draw buttons
	redisplay |= _map_size.draw(alpha);
	redisplay |= _wrap_mode.draw(alpha);
	redisplay |= _rnd_mode.draw(alpha);
	redisplay |= _sound_mode.draw(alpha);
	redisplay |= _prev_theme.draw(alpha);
	redisplay |= _next_theme.draw(alpha);
	redisplay |= _btn_ok.draw(alpha);
	redisplay |= _btn_cancel.draw(alpha);

	return redisplay;
}


bool mode_settings::on_mouse_move(const float x, const float y)
{
	bool redisplay = false;

	redisplay |= _map_size.on_mouse_move(x, y);
	redisplay |= _wrap_mode.on_mouse_move(x, y);
	redisplay |= _rnd_mode.on_mouse_move(x, y);
	redisplay |= _sound_mode.on_mouse_move(x, y);
	redisplay |= _prev_theme.on_mouse_move(x, y);
	redisplay |= _next_theme.on_mouse_move(x, y);
	redisplay |= _btn_ok.on_mouse_move(x, y);
	redisplay |= _btn_cancel.on_mouse_move(x, y);

	return redisplay;

}


bool mode_settings::on_mouse_click(const float x, const float y, const Uint8 btn)
{
	if (!(btn & SDL_BUTTON(SDL_BUTTON_LEFT)))
		return false;

	if (_btn_cancel.cross(x, y))
		return true;
	if (_btn_ok.cross(x, y)) {
		_sett_size = static_cast<level::size>(_map_size.get_state());
		_sett_wrap = _wrap_mode.get_state();
		_sett_rnd = _rnd_mode.get_state();
		_sett_sound = _sound_mode.get_state();
		return true;
	}

	const bool next_theme = _next_theme.cross(x, y);
	const bool prev_theme = _prev_theme.cross(x, y);
	if (next_theme || prev_theme)
		game::load_next_theme(next_theme);
	else if (_wrap_mode.cross(x, y))
		_wrap_mode.invert_state();
	else if (_rnd_mode.cross(x, y))
		_rnd_mode.invert_state();
	else if (_sound_mode.cross(x, y))
		_sound_mode.invert_state();
	else
		_map_size.on_mouse_click(x, y);

	return false;
}


void mode_settings::initialize(const level::size sz, const bool wrap, const bool rndm, const bool sound)
{
	_map_size.set_state(sz);
	_wrap_mode.set_state(wrap);
	_rnd_mode.set_state(rndm);
	_sound_mode.set_state(sound);
	_sett_size = sz;
	_sett_wrap = wrap;
	_sett_rnd = rndm;
	_sett_sound = sound;
}


void mode_settings::reset()
{
	initialize(_sett_size, _sett_wrap, _sett_rnd, _sett_sound);
}
