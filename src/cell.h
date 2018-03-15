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


class cell
{
public:
	cell();

	//! Tube type
	enum ttype {
		tt_none,      ///< Free cell
		tt_half,      ///< Half tube (sender or received installed)
		tt_straight,  ///< Straight tube
		tt_curved,    ///< Curved tube
		tt_joiner     ///< Joiner (T)
	};

	//! Cell type
	enum ctype {
		ct_free,      ///< Free cell
		ct_tube,      ///< Tube
		ct_sender,    ///< Sender
		ct_receiver   ///< Receiver
	};

	//! Connection sides
	enum cside {
		cs_top,       ///< Top
		cs_bottom,    ///< Bottom
		cs_left,      ///< Left
		cs_right      ///< Right
	};

	//! Rotate state
	enum state {
		st_unchanged, ///< Cell state unchanged
		st_rcomplete, ///< Rotation complete
		st_updated    ///< Updating in progress
	};

public:
	/**
	 * Reset cell state
	 */
	void reset();

	/**
	 * Save cell state
	 * \return cell description for serialization
	 */
	unsigned char save() const;

	/**
	 * Load cell state from text
	 * \param state cell description
	 * \return false if state is incorrect
	 */
	bool load(const unsigned char state);

	/**
	 * Reverse lock state of the cell
	 */
	inline void reverse_lock()           { _lock = !_lock; }

	/**
	 * Get current lock state of the cell
	 * \return true if cell is locked
	 */
	inline bool locked() const           { return _lock; }

	/**
	 * Get tube type of the cell
	 * \return tube type
	 */
	inline ttype tube_type() const       { assert(_cell_type != ct_free); return _tube_type; }

	/**
	 * Get cell type of the cell
	 * \return cell type
	 */
	inline ctype cell_type() const       { return _cell_type; }

	/**
	 * Get direction connection properties
	 * \return true if connected
	 */
	inline bool top_connected() const    { return _top_conn; }

	/**
	 * Get direction connection properties
	 * \return true if connected
	 */
	inline bool bottom_connected() const { return _bottom_conn; }

	/**
	 * Get direction connection properties
	 * \return true if connected
	 */
	inline bool left_connected() const   { return _left_conn; }

	/**
	 * Get direction connection properties
	 * \return true if connected
	 */
	inline bool right_connected() const  { return _right_conn; }

	/**
	 * Set Sender cell type of the cell
	 */
	inline void set_type_sender()        { assert(_cell_type == ct_free); _cell_type = ct_sender; }

	/**
	 * Set Receiver cell type of the cell
	 */
	inline void set_type_receiver()      { assert(_cell_type == ct_free); _cell_type = ct_receiver; }

	/**
	 * Get status of the cell
	 * \return cell status (true if connected/active)
	 */
	inline bool active() const           { return _state; }

	/**
	 * Set status of the cell
	 * \param type cell status (true if connected/active)
	 */
	inline void set_active(const bool type) { _state = type; }

	/**
	 * Get current angle of the cell
	 * \return current angle of the cell in degrees
	 */
	inline float angle() const           { return _angle; }

	/**
	 * Add tube to cell
	 * \param side side of the added tube
	 */
	void add_tube(const cside side);

	/**
	 * Check for add tube possibility
	 * \return true if tube can be added
	 */
	bool can_add_tube() const;

	/**
	 * Get current use flag of the cell
	 * \return current use flag of the cell
	 */
	inline bool is_used() const           { return _used; }

	/**
	 * Set current use flag of the cell
	 * \param weight a new use flag of the cell
	 */
	inline void set_used(const bool used) { _used = used; }

	/**
	 * Start rotation
	 * \param dir rotation direction (true = clockwise)
	 */
	void rotate(const bool dir);

	/**
	 * Start still rotation (without animation)
	 * \param dir rotation direction (true = clockwise)
	 * \param twice rotation mode (true = 180, false = 90 degree)
	 */
	void rotate_still(const bool dir, const bool twice);

	/**
	 * Check if rotation in progress
	 * \return true if rotation in progress
	 */
	inline bool rotation_in_progress() const { return _rotate.start_time != 0; }

	/**
	 * Calculate rotation angle
	 * \return true if rotation has been finished
	 */
	state calculate_state();

private:
	/**
	 * Calculate state on rotation complete
	 */
	void update_rotate_state();

	/**
	 * Get connected side count
	 * \return connected side count
	 */
	unsigned char tube_side_count() const;

private:
	ttype _tube_type;   ///< Tube type
	ctype _cell_type;   ///< Cell type
	float _angle;       ///< Angle
	bool  _state;       ///< Cell connection state (true = active, false = passive)
	bool  _lock;        ///< Cell lock status
	bool  _top_conn;    ///< Top connection state
	bool  _bottom_conn; ///< Bottom connection state
	bool  _left_conn;   ///< Left connection state
	bool  _right_conn;  ///< Right connection state
	bool  _used;        ///< Cell route use flag

	//Rotation description
	struct rotation {
		rotation() : start_time(0) {}
		bool         direction;  ///< Rotate direction
		unsigned int start_time; ///< Rotate start time
		bool         twice;      ///< Rotate twice flag (180 degree)
		float        init_angle; ///< Init rotate angle
		float        need_angle; ///< Needed rotate angle
	} _rotate;
};
