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

#include "common.h"
#include "mode_puzzle.h"
#include "mode_settings.h"


/**
 * Game class
 */
class game
{
private:
	game();

public:
	/**
	 * Get class instance
	 * \return class instance
	 */
	static game& instance();

	/**
	 * Initialization
	 * \param lvl_id start level id (0 to use settings file)
	 * \param lvl_sz start level map size (if lvl_id > 0)
	 * \param lvl_wrap start level wrap mode (if lvl_id > 0)
	 * \return false on error
	 */
	bool initialize(const unsigned long lvl_id, const level::size lvl_sz, const bool lvl_wrap);

	/**
	 * Finalization
	 */
	void finalize();

	/**
	 * Get redisplay flag
	 * \return redisplay flag (true if we need to redraw window)
	 */
	bool need_redisplay() const { return _need_redisplay; }

	/**
	 * Render scene
	 */
	void draw_scene();

	/**
	 * Mouse move handler
	 * \param x new mouse X window coordinate
	 * \param y new mouse Y window coordinate
	 */
	void on_mouse_move(const int x, const int y);

	/**
	 * Mouse click handler
	 * \param btn mouse button identifier
	 */
	void on_mouse_click(const Uint8 btn);

	/**
	 * Keyboard key press handler
	 * \param key key identifier
	 * \return true if application finished (exit)
	 */
	bool on_key_press(const SDLKey key);

	/**
	 * Window resize handler
	 * \param width new window width size
	 * \param height new window height size
	 */
	void on_window_resize(const int width, const int height);

	/**
	 * Load next theme
	 * \param direction true to next, false to previous theme file
	 * \return false if no one theme available
	 */
	static bool load_next_theme(const bool direction);

private:
	/**
	 * Swap modes
	 */
	void swap_mode();

private:
	bool  _need_redisplay;      ///< Redisplay flag
	int   _wnd_width;           ///< Game window width
	int   _wnd_height;          ///< Game window height
	float _mouse_x;             ///< Last mouse X world coordinate
	float _mouse_y;             ///< Last mouse Y world coordinate

	mode_puzzle   _mode_puzzle; ///< Puzzle (game) mode handler
	mode_settings _mode_sett;   ///< Settings mode handler
	mode*         _curr_mode;   ///< Currently active mode
	mode*         _next_mode;   ///< Next mode
	unsigned int  _trans_stime; ///< Transition start time (zero if transition is not active)
};
