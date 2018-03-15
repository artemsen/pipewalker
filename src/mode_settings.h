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
#include "button.h"
#include "mode.h"
#include "level.h"


class mode_settings : public mode
{
public:
	mode_settings();

	//From mode class
	bool draw(const float alpha);
	bool on_mouse_move(const float x, const float y);
	bool on_mouse_click(const float x, const float y, const Uint8 btn);

	/**
	 * Initialize mode
	 * \param sz showed level size
	 * \param wrap wrap mode in/off
	 * \param rndm random mode in/off
	 * \param sound sound mode in/off
	 */
	void initialize(const level::size sz, const bool wrap, const bool rndm, const bool sound);

	/**
	 * Reset state before show
	 */
	void reset();

	//Settings accessors
	level::size level_size() const { return _sett_size; }
	bool wrap_mode() const         { return _sett_wrap; }
	bool random_mode() const       { return _sett_rnd; }
	bool sound_mode() const        { return _sett_sound; }

private:
	button          _btn_ok;     ///< OK button
	button          _btn_cancel; ///< Cancel button
	button_radio<4> _map_size;   ///< Map size radio buttons group
	button_chbox    _wrap_mode;  ///< Wrapping mode on/off checkbox
	button_chbox    _rnd_mode;   ///< Random on/off checkbox
	button_chbox    _sound_mode; ///< Sound on/off checkbox
	button          _prev_theme; ///< Previous theme button
	button          _next_theme; ///< Next theme button
	string          _theme_name; ///< Current theme name

	level::size     _sett_size;  ///< Current settings: level size
	bool            _sett_wrap;  ///< Current settings: wrap mode
	bool            _sett_rnd;   ///< Current settings: random level mode
	bool            _sett_sound; ///< Current settings: sound mode
};
