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

#include "cell.h"

#define PW_MAX_LEVEL_NUMBER 99999999


class level
{
public:
	level();

	//! Map sizes
	enum size {
		sz_small  = 0,
		sz_normal,
		sz_big,
		sz_extra
	};

	/**
	 * Initialize new random map
	 * \param id map Id
	 * \param sz map size
	 * \param wrap_mode wrap mode on/off flag
	 */
	void create(const unsigned long id, const size sz, const bool wrap_mode);

	/**
	 * Load map state from string
	 * \param id map Id
	 * \param sz level map size
	 * \param wrap_mode wrap mode on/off
	 * \param state level state description
	 * \return false if loading fail
	 */
	bool load(const unsigned long id, const size sz, const bool wrap_mode, const string& state);

	/**
	 * Save map state as text
	 * \return map state
	 */
	string save() const;

	/**
	 * Rotate specified cell
	 * \param x an X coordinate of cell
	 * \param y an Y coordinate of cell
	 * \param dir rotation direction (true = clockwise)
	 */
	void rotate(const unsigned short x, const unsigned short y, const bool dir);

	/**
	 * Rotate all cells (reset map)
	 */
	void rotate_all();

	/**
	 * Calculate rotation angle
	 * \return true if rotation has been finished
	 */
	cell::state calculate_state();

	/**
	 * Get "game over" flag
	 * \return "game over" flag
	 */
	inline bool solved() const	{ return _solved; }

	/**
	 * Get object's cell
	 * \param x an X coordinate of cell
	 * \param y an Y coordinate of cell
	 * \return pointer to object cell
	 */
	const cell& get_cell(const unsigned short x, const unsigned short y) const;

	/**
	 * Reverse lock
	 * \param x an X coordinate of cell
	 * \param y an Y coordinate of cell
	 */
	void reverse_lock(const unsigned short x, const unsigned short y);

	/**
	 * Get current map size (number of col/row)
	 * \return current map size
	 */
	inline unsigned short get_csize() const { return _size_cell; }

	/**
	 * Get current map size type
	 * \return current map size type
	 */
	inline size get_tsize() const           { return _size_type; }

	/**
	 * Get current map ID
	 * \return current map ID
	 */
	inline unsigned long id() const         { return _id; }

	/**
	 * Get current map ID
	 * \return current map ID
	 */
	inline const char* id_text() const      { return _id_text; }

	/**
	 * Get current wrap mode
	 * \return wrap mode
	 */
	inline bool wrap_mode() const           { return _wrap_mode; }

	/**
	 * Get map size in col/row number by type
	 * \param sz map type
	 * \return col/row size
	 */
	static unsigned short size_from_type(const size sz);

private:
	/**
	 * Get object's cell
	 * \param x an X coordinate of cell
	 * \param y an Y coordinate of cell
	 * \return pointer to object cell
	 */
	cell& cell_at(const unsigned short x, const unsigned short y);

	/**
	* Define connection status
	*/
	void define_connect_status();

	/**
	 * Install receiver on map
	 * \return result (false if it is unable to install)
	 */
	bool install_receiver();

	/**
	 * Install sender on map
	 */
	void install_sender();

	/**
	 * Make route from receiver to sender
	 * \param x an X coordinate of cell
	 * \param y an Y coordinate of cell
	 * \return result (false if route is unaccessible)
	 */
	bool make_route(const unsigned short x, const unsigned short y);

	/**
	 * Make connection between two cell
	 * \param curr_x an X coordinate of source cell
	 * \param curr_y an Y coordinate of source cell
	 * \param next_x an X coordinate of destination cell
	 * \param next_y an Y coordinate of destination cell
	 */
	void make_connection(const unsigned short curr_x, const unsigned short curr_y, const unsigned short next_x, const unsigned short next_y);

	/**
	 * Define connection status
	 * \param x an X coordinates of latest connected cell
	 * \param y an Y coordinates of latest connected cell
	 */
	void define_connect_status(const unsigned short x, const unsigned short y);


private:
	typedef vector<cell> cells;

	unsigned long   _id;                ///< Map ID
	char            _id_text[9];        ///< Map ID (as text)
	size            _size_type;         ///< Map size (type)
	unsigned short  _size_cell;         ///< Map size (cell count)
	cells           _cells;             ///< Object's map
	bool            _solved;            ///< State flag: true if all receivers connected
	bool            _wrap_mode;         ///< Wrapping mode on/off flag
	unsigned short  _senderX, _senderY; ///< Sender coordinates
	unsigned short  _zeroX, _zeroY;     ///< Zero point coordinates (sender out)
};
