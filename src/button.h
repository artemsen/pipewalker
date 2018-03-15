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

#pragma once

#include "common.h"
#include "render.h"


/**
 * Simple button
 */
class button
{
public:
	button() {}

	/**
	 * Button initialization
	 * \param img_n button image identifier (normal)
	 * \param img_s button image identifier (shadow)
	 * \param x button center x coordinate
	 * \param y button center y coordinate
	 * \param scale button scale
	 * \param id button ID
	 */
	button(const render::img_type img_n, const render::img_type img_s, const float x, const float y, const float scale = 1.0f, const int id = 0);

	/**
	 * Button setup
	 * \param img_n button image identifier (normal)
	 * \param img_s button image identifier (shadow)
	 * \param x button center x coordinate
	 * \param y button center y coordinate
	 * \param scale button scale
	 * \param id button ID
	 */
	void setup(const render::img_type img_n, const render::img_type img_s, const float x, const float y, const float scale = 1.0f, const int id = 0);

	/**
	 * Check for cross specified point with button's area
	 * \param x position on the X axis
	 * \param y position on the Y axis
	 * \return true if point is over button
	 */
	inline bool cross(const float x, const float y) const { return (x >= _coord_x - _scale / 2.0f && x <= _coord_x + _scale / 2.0f && y <= _coord_y + _scale / 2.0f && y >= _coord_y - _scale / 2.0f); }

	/**
	 * Get button Id
	 * \return button Id
	 */
	inline int id() const { return _id; }

	/**
	 * Mouse move handler
	 * \param x position of the mouse cursor on the X axis
	 * \param y position of the mouse cursor on the Y axis
	 * \return true if cursor enter or leave button's area (usually we have to redraw button)
	 */
	bool on_mouse_move(const float x, const float y);

	/**
	 * Draw button
	 * \param alpha alpha channel in range [0.0f ... 1.0f]
	 * \return true if redisplay needed
	 */
	virtual bool draw(const float alpha = 1.0f);

protected:
	render::img_type _img_n;      ///< Button texture identifier (normal)
	render::img_type _img_s;      ///< Button texture identifier (shadow)
	float            _coord_x;    ///< Button x coordinate
	float            _coord_y;    ///< Button y coordinate
	float            _scale;      ///< Button scale
	int              _id;         ///< Button Id
	unsigned long    _shadow_tr;  ///< Shadow drawing transition start time
	bool             _mouse_over; ///< Mouse cursor over button flag
};


/**
 * Check box button
 */
class button_chbox : public button
{
public:
	button_chbox() {}

	/**
	 * Button initialization
	 * \param state initial button state (on/off)
	 * \param x button center x coordinate
	 * \param y button center y coordinate
	 * \param scale button scale
	 * \param id button ID
	 */
	button_chbox(const bool state, const float x, const float y, const float scale, const int id = 0);

	/**
	 * Button initialization
	 * \param state initial button state (on/off)
	 * \param x button center x coordinate
	 * \param y button center y coordinate
	 * \param scale button scale
	 * \param id button ID
	 */
	void setup(const bool state, const float x, const float y, const float scale, const int id);

	/**
	 * Set button state (without transition effect)
	 * \param state new state
	 */
	void set_state(const bool state);

	/**
	 * Invert button state (with transition effect)
	 */
	void invert_state();

	/**
	 * Get button state
	 * \return button state
	 */
	inline bool get_state() const			{ return _state; }

	//From button class
	virtual bool draw(const float alpha = 1.0f);

protected:
	bool          _state;       ///< Button state (on/off)
	unsigned long _state_trans; ///< State transition start time
};


/**
 * Radio buttons group
 */
template<size_t N> class button_radio
{
public:
	/**
	 * Render radio button group
	 * \param alpha alpha channel in range [0.0f ... 1.0f]
	 * \return true if redisplay needed
	 */
	bool draw(const float alpha = 1.0f)
	{
		bool redisplay = false;
		for (size_t i = 0; i < N; ++i)
			redisplay |= _buttons[i].draw(alpha);
		return redisplay;
	}

	/**
	 * Mouse move handler
	 * \param x position of the mouse cursor on the X axis
	 * \param y position of the mouse cursor on the Y axis
	 * \return true if cursor enter or leave button's area (usually we have to redraw button)
	 */
	bool on_mouse_move(const float x, const float y)
	{
		bool redispay = false;
		for (size_t i = 0; i < N; ++i)
			redispay |= _buttons[i].on_mouse_move(x, y);
		return redispay;
	}


	/**
	 * Mouse click handler
	 * \param x position on the X axis
	 * \param y position on the Y axis
	 * \return true if state has been changed
	 */
	bool on_mouse_click(const float x, const float y)
	{
		size_t clicked_id;
		for (clicked_id = 0; clicked_id < N; ++clicked_id) {
			if (_buttons[clicked_id].cross(x, y))
				break;
		}
		if (clicked_id == N)
			return false;	//Not in buttons area

		size_t old_checked;
		for (old_checked = 0; old_checked < N; ++old_checked) {
			if (_buttons[old_checked].get_state())
				break;
		}
		if (clicked_id != old_checked) {
			_buttons[clicked_id].invert_state();
			_buttons[old_checked].invert_state();
			return true;
		}

		return false;
	}

	/**
	 * Get current choice id
	 * \return current choice id
	 */
	int get_state() const
	{
		int id = -1;
		for (size_t i = 0; id < 0 && i < N; ++i)
			if (_buttons[i].get_state())
				id = _buttons[i].id();
		return id;
	}

	/**
	 * Set current choice id
	 * \param id current choice id
	 */
	void set_state(const int id)
	{
		for (size_t i = 0; i < N; ++i)
			_buttons[i].set_state(_buttons[i].id() == id);
	}

	//Accessors
	button_chbox& operator [] (size_t i)
	{
		assert(i < N);
		return _buttons[i];
	}

private:
	button_chbox _buttons[N]; ///< Buttons group
};
