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

#include "button.h"

#define PW_BTN_CB_STATE_SPEED      250
#define PW_BTN_SHADOW_STATE_SPEED  500


button::button(const render::img_type img_n, const render::img_type img_s, const float x, const float y, const float scale /*= 1.0f*/, const int id /* = 0*/)
:	_img_n(img_n), _img_s(img_s),
	_coord_x(x), _coord_y(y),
	_scale(scale),
	_id(id),
	_shadow_tr(0),
	_mouse_over(false)
{
}


void button::setup(const render::img_type img_n, const render::img_type img_s, const float x, const float y, const float scale /*= 1.0f*/, const int id /*= 0*/)
{
	_img_n = img_n;
	_img_s = img_s;
	_coord_x = x;
	_coord_y = y;
	_scale = scale;
	_id = id;
}


bool button::on_mouse_move(const float x, const float y)
{
	const bool new_over_state = cross(x, y);
	if (_mouse_over != new_over_state) {
		_mouse_over = new_over_state;
		_shadow_tr = SDL_GetTicks();
		return true;
	}
	return false;
}


bool button::draw(const float alpha /*= 1.0f*/)
{
	render& renderer = render::instance();
	bool shadow_trans = (_shadow_tr != 0);
	float alpha_shadow = _mouse_over ? alpha : 0.0f;

	if (shadow_trans) {
		const unsigned int diff_time = SDL_GetTicks() - _shadow_tr;
		if (diff_time > PW_BTN_SHADOW_STATE_SPEED)
			_shadow_tr = 0;
		else {
			alpha_shadow = static_cast<float>(diff_time) / static_cast<float>(PW_BTN_SHADOW_STATE_SPEED);
			if (!_mouse_over)
				alpha_shadow = 1.0f - alpha_shadow;
			alpha_shadow = min(alpha, alpha_shadow);
		}
	}

	renderer.draw(_img_s, _coord_x, _coord_y, _scale + _scale / 2.5f, alpha_shadow);
	renderer.draw(_img_n, _coord_x, _coord_y, _scale, alpha);

	return shadow_trans;
}


button_chbox::button_chbox(const bool state, const float x, const float y, const float scale, const int id /* = 0 */)
:	button(_state ? render::btn_cb_on : render::btn_cb_off, _state ? render::btn_cb_on_s : render::btn_cb_off_s, x, y, scale, id),
	_state(state), _state_trans(0)
{
}


void button_chbox::setup(const bool state, const float x, const float y, const float scale, const int id)
{
	_img_n = _state ? render::btn_cb_on : render::btn_cb_off;
	_img_s = _state ? render::btn_cb_on_s : render::btn_cb_off_s;
	_coord_x = x;
	_coord_y = y;
	_scale = scale;
	_id = id;
	_state = state;
}


void button_chbox::set_state(const bool state)
{
	_state = state;
	_img_n = _state ? render::btn_cb_on : render::btn_cb_off;
	_img_s = _state ? render::btn_cb_on_s : render::btn_cb_off_s;
}


void button_chbox::invert_state()
{
	set_state(!_state);
	_state_trans = SDL_GetTicks();
}


bool button_chbox::draw(const float alpha /*= 1.0f*/)
{
	float alpha_tr = alpha;
	bool redisplay = (_state_trans != 0);

	if (redisplay) {
		const unsigned int diff_time = SDL_GetTicks() - _state_trans;
		if (diff_time > PW_BTN_CB_STATE_SPEED)
			_state_trans = 0;
		else {
			alpha_tr = static_cast<float>(diff_time) / static_cast<float>(PW_BTN_CB_STATE_SPEED);
			_img_n = _state ? render::btn_cb_off : render::btn_cb_on;
			_img_s = _state ? render::btn_cb_off_s : render::btn_cb_on_s;
			button::draw(1.0f - alpha_tr);
			_img_n = _state ? render::btn_cb_on : render::btn_cb_off;
			_img_s = _state ? render::btn_cb_on_s : render::btn_cb_off_s;
		}
	}

	redisplay |= button::draw(alpha_tr);

	return redisplay;
}
