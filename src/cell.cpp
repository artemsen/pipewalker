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

#include "cell.h"

#define PW_ROTATE_TUBE_SPEED     300


cell::cell()
{
	reset();
}


void cell::reset()
{
	_tube_type = tt_none;
	_cell_type = ct_free;
	_angle = 0.0f;
	_state = false;
	_lock = false;
	_top_conn = false;
	_bottom_conn = false;
	_left_conn = false;
	_right_conn = false;
	_used = false;
}


unsigned char cell::save() const
{
	const int rotate = static_cast<int>(rotation_in_progress() ? _rotate.init_angle / 90.0f : _angle / 90.0f);
	assert(rotate >= 0 && rotate <= 3);

	return
		(static_cast<unsigned char>(_tube_type) << 5) |
		(static_cast<unsigned char>(_cell_type) << 3) |
		(static_cast<unsigned char>(rotate)     << 1) |
		(_lock ? 1 : 0);
}


bool cell::load(const unsigned char state)
{
	_tube_type = static_cast<ttype>((state >> 5) & 0x7);
	_cell_type = static_cast<ctype>((state >> 3) & 0x3);
	const int rotate = (state >> 1) & 0x3;
	_lock = (state >> 0) & 0x1;

	_angle = rotate * 90.0f;

	//Restore connection side by rotate angle
	switch (rotate) {
		case 0:	//0 degrees
			switch (_tube_type) {
				case tt_none:														break;
				case tt_half:		_top_conn = true;								break;
				case tt_straight:	_top_conn = _bottom_conn = true;				break;
				case tt_curved:		_top_conn = _right_conn = true;					break;
				case tt_joiner:		_top_conn = _bottom_conn = _right_conn = true;	break;
				default:			assert(false && "Unknown tube type"); return false;
			}
			break;
		case 1:	//90 degrees
			switch (_tube_type) {
				case tt_none:														break;
				case tt_half:		_left_conn = true;								break;
				case tt_straight:	_right_conn = _left_conn = true;				break;
				case tt_curved:		_top_conn = _left_conn = true;					break;
				case tt_joiner:		_top_conn = _left_conn = _right_conn = true;	break;
				default:			assert(false && "Unknown tube type"); return false;
			}
			break;
		case 2:	//180 degrees
			switch (_tube_type) {
				case tt_none:														break;
				case tt_half:		_bottom_conn = true;							break;
				case tt_straight:	_top_conn = _bottom_conn = true;				break;
				case tt_curved:		_bottom_conn = _left_conn = true;				break;
				case tt_joiner:		_top_conn = _bottom_conn = _left_conn = true;	break;
				default:			assert(false && "Unknown tube type"); return false;
			}
			break;
		case 3:	//270 degrees
			switch (_tube_type) {
				case tt_none:														break;
				case tt_half:		_right_conn = true;								break;
				case tt_straight:	_right_conn = _left_conn = true;				break;
				case tt_curved:		_right_conn = _bottom_conn = true;				break;
				case tt_joiner:		_right_conn = _left_conn = _bottom_conn = true;	break;
				default:			assert(false && "Unknown tube type"); return false;
			}
			break;
		default:
			assert(false && "Wrong angle"); return false;
	}

	return true;
}


void cell::add_tube(const cside side)
{
	assert(can_add_tube());

	//Already connected?
	assert(!(_top_conn && side == cs_top));
	assert(!(_bottom_conn && side == cs_bottom));
	assert(!(_left_conn && side == cs_left));
	assert(!(_right_conn && side == cs_right));

	//Add connection
	switch (side) {
		case cs_top:	_top_conn = true;		break;
		case cs_bottom:	_bottom_conn = true;	break;
		case cs_left:	_left_conn = true;		break;
		case cs_right:	_right_conn = true;		break;
		default:
			assert(false && "Undefined connection side");
			break;
	}
	//Define cell type
	if (_cell_type == ct_free)
		_cell_type = ct_tube;

	//Define tube type
	switch (tube_side_count()) {
		case 1:	_tube_type = tt_half;	break;
		case 2:	_tube_type = (_top_conn || _bottom_conn) && (_left_conn || _right_conn) ? tt_curved : tt_straight;	break;
		case 3:	_tube_type = tt_joiner;	break;
		default:
			assert(false && "Incorrect connection count");
			break;
	}

	//Define angle from base direction (anticlockwise)
	_angle = 0.0f;
	switch (_tube_type) {
		case tt_half:
			if (_right_conn)									_angle = 90.0f * 3;
			else if (_bottom_conn)								_angle = 90.0f * 2;
			else if (_left_conn)								_angle = 90.0f * 1;
			break;
		case tt_straight:
			if (_right_conn /*|| _ConnLeft*/)					_angle = 90.0f * 1;
			break;
		case tt_curved:
			if (_right_conn && _bottom_conn)					_angle = 90.0f * 3;
			else if (_bottom_conn && _left_conn)				_angle = 90.0f * 2;
			else if (_left_conn && _top_conn)					_angle = 90.0f * 1;
			break;
		case tt_joiner:
			if (_left_conn && _right_conn && _bottom_conn)		_angle = 90.0f * 3;
			else if (_top_conn && _bottom_conn && _left_conn)	_angle = 90.0f * 2;
			else if (_left_conn && _right_conn && _top_conn)	_angle = 90.0f * 1;
			break;
		default:
			assert(false && "Unknown tube type");
			break;
	}
}


bool cell::can_add_tube() const
{
	const unsigned char connection_count = tube_side_count();
	return
		_cell_type == ct_free ||
		(_cell_type == ct_tube && connection_count < 3) ||
		((_cell_type == ct_sender || _cell_type == ct_receiver) && connection_count == 0);
}


unsigned char cell::tube_side_count() const
{
	unsigned char connection_count = 0;
	if (_top_conn)
		connection_count++;
	if (_bottom_conn)
		connection_count++;
	if (_left_conn)
		connection_count++;
	if (_right_conn)
		connection_count++;
	return connection_count;
}


void cell::rotate(const bool dir)
{
	if (locked())
		return;
	if (rotation_in_progress()) {
		if (dir == _rotate.direction)
			_rotate.twice = true;
		else {
			//Back rotation
			const bool new_conn_top =    _rotate.direction ? _left_conn : _right_conn;
			const bool new_conn_bottom = _rotate.direction ? _right_conn : _left_conn;
			const bool new_conn_left =   _rotate.direction ? _bottom_conn : _top_conn;
			const bool new_conn_right =  _rotate.direction ? _top_conn : _bottom_conn;
			_top_conn = new_conn_top;
			_bottom_conn = new_conn_bottom;
			_left_conn = new_conn_left;
			_right_conn = new_conn_right;
			_rotate.direction = !_rotate.direction;
			_rotate.twice = false;
			const unsigned long curr_tick = SDL_GetTicks();
			_rotate.start_time = curr_tick - (PW_ROTATE_TUBE_SPEED - (curr_tick - _rotate.start_time));
			_rotate.need_angle = _rotate.init_angle;
			_rotate.init_angle += (_rotate.direction ? 90.0f : -90.0f);
		}
	}
	else {
		_rotate.direction = dir;
		_rotate.start_time = SDL_GetTicks();
		_rotate.twice = false;
		_rotate.init_angle = _angle;
		_rotate.need_angle = _angle + (_rotate.direction ? -90.0f : 90.0f);
	}
}


void cell::rotate_still(const bool dir, const bool twice)
{
	assert(!locked());

	_rotate.direction = dir;
	_rotate.need_angle = _angle + (dir ? -90.0f : 90.0f);
	update_rotate_state();
	if (twice)
		rotate_still(dir, false);
}


cell::state cell::calculate_state()
{
	state st = st_unchanged;

	//Rotation processing
	if (rotation_in_progress()) {
		const unsigned int diff_time = SDL_GetTicks() - _rotate.start_time;
		if (diff_time < PW_ROTATE_TUBE_SPEED) {
			const float degree = static_cast<float>(diff_time) / static_cast<float>(PW_ROTATE_TUBE_SPEED) * 90.0f;
			_angle = _rotate.init_angle;
			_angle += (_rotate.direction ? -degree : degree);
			st = st_updated;
		}
		else {
			update_rotate_state();
			if (!_rotate.twice)
				st = st_rcomplete;
			else {
				rotate(_rotate.direction);
				st = st_updated;
			}
		}
	}

	return st;
}


void cell::update_rotate_state()
{
	_angle = _rotate.need_angle;
	if (_angle > 359.0f)
		_angle = 0.0f;
	else if (_angle < 0.0f)
		_angle = 270.0f;
	_rotate.start_time = 0;

	//Calculate new connection sides
	const bool new_conn_top =    _rotate.direction ? _left_conn : _right_conn;
	const bool new_conn_bottom = _rotate.direction ? _right_conn : _left_conn;
	const bool new_conn_left =   _rotate.direction ? _bottom_conn : _top_conn;
	const bool new_conn_right =  _rotate.direction ? _top_conn : _bottom_conn;
	_top_conn = new_conn_top;
	_bottom_conn = new_conn_bottom;
	_left_conn = new_conn_left;
	_right_conn = new_conn_right;
}
