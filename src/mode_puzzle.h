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
#include "explosion.h"
#include "button.h"
#include "mode.h"
#include "level.h"


class mode_puzzle : public mode
{
public:
	mode_puzzle();

	//From mode class
	bool draw(const float alpha);
	bool on_mouse_move(const float x, const float y);
	bool on_mouse_click(const float x, const float y, const Uint8 btn);

	/**
	 * Initialization
	 */
	void initialize();

	/**
	 * Get current level size
	 * \return current level size
	 */
	level::size current_level_size() const { return _curr_level->get_tsize(); }

	/**
	 * Get current wrap mode
	 * \return current wrap mode
	 */
	bool current_wrap_mode() const         { return _curr_level->wrap_mode(); }

	/**
	 * Settings change handle
	 * \param sound sound mode in/off
	 * \param wrap wrap mode in/off
	 */
	void on_settings_changed(const level::size sz, const bool wrap);

	/**
	 * Save current level to settings
	 */
	void save_current_level() const;

private:
	/**
	 * Draw level puzzle
	 * \param lvl rendered level
	 * \param alpha transparency [0.0f ... 1.0f]
	 */
	void draw_level(const level& lvl, const float alpha) const;

	/**
	 * Switch to specified level
	 * \param id level id
	 * \param sz level size
	 * \param wrap_mode level wrap mode
	 */
	void switch_to_level(const unsigned long id, const level::size sz, const bool wrap_mode);

private:
	explosion    _win_anim;     ///< Winner animation
	button       _btn_reset;    ///< Reset button
	button       _btn_prev;     ///< Previous level button
	button       _btn_next;     ///< Next level button
	button       _btn_sett;     ///< Settings button

	unsigned int _trans_stime;  ///< Transition start time (zero if transition is not active)
	level        _levels[2];    ///< Game maps (current and next)
	level*       _curr_level;   ///< Currently active level
	level*       _next_level;   ///< Next level
};
