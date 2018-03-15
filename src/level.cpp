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

#include "level.h"
#include "mtrandom.h"
#include "settings.h"

#define C2H(x) (x >= '0' && x <= '9' ? x - '0' : x >= 'A' && x <= 'F' ? x - ('A' - 10) : x - ('a' - 10))


level::level()
:	_id(0),
	_size_cell(10),
	_solved(false),
	_wrap_mode(true),
	_senderX(0), _senderY(0),
	_zeroX(0), _zeroY(0)
{
}


void level::create(const unsigned long id, const size sz, const bool wrap_mode)
{
 	assert(id >= 1 && id <= PW_MAX_LEVEL_NUMBER);

	_id = id;
	sprintf(_id_text, "%08u", static_cast<unsigned int>(_id));
	_wrap_mode = wrap_mode;
	_size_type = sz;
	_size_cell = size_from_type(sz);

	_cells.resize(_size_cell * _size_cell);

	mtrandom::seed(id);		//Initialize random sequence

	const unsigned int receivers_num = (_size_cell * _size_cell) / 5;

	bool install_done = false;
	while (!install_done) {
		//Reset map
		for (cells::iterator it = _cells.begin(); it != _cells.end(); ++it)
			it->reset();

		install_sender();

		//Install receivers
		install_done = true;
		for (unsigned int i = 0; i < receivers_num && install_done; ++i)
			install_done &= install_receiver();
	}

	if (settings::debug_mode())
		_solved = false;
	else {
		//Reset map - still rotation
		for (cells::iterator it = _cells.begin(); it != _cells.end(); ++it) {
			if (it->cell_type() != cell::ct_free && mtrandom::random(0, 5) != 0) {
				it->rotate_still(mtrandom::random(0, 2) != 0, mtrandom::random(0, 2) != 0);
			}
		}
		define_connect_status();
	}
}


bool level::load(const unsigned long id, const size sz, const bool wrap_mode, const string& state)
{
	assert(id >= 1 && id <= PW_MAX_LEVEL_NUMBER);
	assert(!state.empty());

	_id = id;
	sprintf(_id_text, "%08u", static_cast<unsigned int>(_id));
	_wrap_mode = wrap_mode;
	_size_type = sz;
	_size_cell = size_from_type(sz);
	const size_t cell_count = _size_cell * _size_cell;
	_cells.resize(cell_count);

	if (state.length() != cell_count * 2)
		return false;

	for (size_t i = 0; i < cell_count; ++i) {
		_cells[i].reset();

		unsigned char cell_state;
		cell_state = C2H(state[i * 2]);
		cell_state = cell_state << 4;
		cell_state |= C2H(state[i * 2 + 1]);

		if (!_cells[i].load(cell_state))
			return false;
	}

	for (unsigned short y = 0; y < _size_cell; ++y) {
		for (unsigned short x = 0; x < _size_cell; ++x) {
			cell& c = cell_at(x, y);
			if (c.cell_type() == cell::ct_sender) {
				_senderX = x;
				_senderY = y;
				break;
			}
		}
	}

	define_connect_status();
	_solved = false;

	return true;
}


string level::save() const
{
	string state;

	for (unsigned short y = 0; y < _size_cell; ++y) {
		for (unsigned short x = 0; x < _size_cell; ++x) {
			const unsigned char cell_state = get_cell(x, y).save();
			char buf[3];
			sprintf(buf, "%02x", cell_state);
			state += buf;
		}
	}

	return state;
}


void level::rotate_all()
{
	for (cells::iterator it = _cells.begin(); it != _cells.end(); ++it) {
		if (it->cell_type() != cell::ct_free && !it->locked() && mtrandom::random(0, 5) != 0) {
			it->rotate(mtrandom::random(0, 2) != 0);
			if (mtrandom::random(0, 2) != 0) //Twice rotation
				it->rotate(true /* ignored in twice rotation mode */);
		}
	}
	define_connect_status();
}


void level::install_sender()
{
	do {
		_senderX = static_cast<unsigned short>(mtrandom::random(0, static_cast<int>(_size_cell)));
		_senderY = static_cast<unsigned short>(mtrandom::random(0, static_cast<int>(_size_cell)));

	} while (!_wrap_mode && (_senderX == 0 || _senderX == _size_cell - 1 || _senderY == 0 || _senderY == _size_cell - 1));


	cell& srv = cell_at(_senderX, _senderY);
	srv.set_type_sender();

	//Define zero point (sender output cell)
	_zeroX = _senderX;
	_zeroY = _senderY;
	switch (mtrandom::random(0, 4)) {
		case 0:
			_zeroX = (_zeroX + 1) % _size_cell;
			break;
		case 1:
			_zeroX = _zeroX == 0 ? _size_cell - 1 : _zeroX - 1;
			break;
		case 2:
			_zeroY = (_zeroY + 1) % _size_cell;
			break;
		case 3:
			_zeroY = _zeroY == 0 ? _size_cell - 1 : _zeroY - 1;
			break;
		default:
			assert(false);
			break;
	}

	make_connection(_senderX, _senderY, _zeroX, _zeroY);
}


cell& level::cell_at(const unsigned short x, const unsigned short y)
{
	assert(x < _size_cell && y < _size_cell);	//Check for outside the map
	return _cells[x + y * _size_cell];
}


const cell& level::get_cell(const unsigned short x, const unsigned short y) const
{
	assert(x < _size_cell && y < _size_cell);	//Check for outside the map
	return _cells[x + y * _size_cell];
}


bool level::install_receiver()
{
	//Get free cells
	vector< pair<unsigned short, unsigned short> > free_cells;
	for (unsigned short x = 0; x < _size_cell; ++x) {
		for (unsigned short y = 0; y < _size_cell; ++y) {
			cell& c = cell_at(x, y);
			if (c.cell_type() == cell::ct_free)
				free_cells.push_back(make_pair(x, y));
			c.set_used(false);

		}
	}
	if (free_cells.empty())
		return true;	//No more free cells

	bool result = false;

	int try_counter = _size_cell * 2;
	while (try_counter-- && !result) {
		//Backup current map state
		const cells backup = _cells;
		const int free_cell_ind = mtrandom::random(0, static_cast<int>(free_cells.size()));
		const unsigned short free_x = free_cells[free_cell_ind].first;
		const unsigned short free_y = free_cells[free_cell_ind].second;
		cell& rcv = cell_at(free_x, free_y);
		rcv.set_type_receiver();

		result = make_route(free_x, free_y);

		if (!result)	//Restore map
			_cells = backup;
	}

	return result;
}


bool level::make_route(const unsigned short x, const unsigned short y)
{
	unsigned short next_x = 0, next_y = 0;	//Next coordinates

	bool result = false;

	int try_counter = 5;
	while (try_counter && !result) {
		short i, j;
		do {
			i = 1 - static_cast<unsigned short>(mtrandom::random(0, 3));
			j = 1 - static_cast<unsigned short>(mtrandom::random(0, 3));
		} while ((i && j) || (!i && !j));	//Diagonal


		if (!_wrap_mode) {
			if ((j < 0 && x == 0) || (j > 0 && x == _size_cell - 1) ||
				(i < 0 && y == 0) || (i > 0 && y == _size_cell - 1)) {
					--try_counter;
					continue;
			}
		}

		const unsigned short cp_x = (j < 0 && x == 0) ? _size_cell - 1 : (x + j) % _size_cell;
		const unsigned short cp_y = (i < 0 && y == 0) ? _size_cell - 1 : (y + i) % _size_cell;

		cell& cell = cell_at(cp_x, cp_y);
		if (!cell.is_used() && cell.can_add_tube()) {
			result = true;
			next_x = cp_x;
			next_y = cp_y;
		}

		--try_counter;
	}

	if (!result)
		return false;	//min point - we don't have a route

	make_connection(x, y, next_x, next_y);
	cell_at(x, y).set_used(true);

	if (cell_at(next_x, next_y).tube_type() == cell::tt_joiner || (next_x == _zeroX && next_y == _zeroY))
		return true;

	return make_route(next_x, next_y);
}


void level::make_connection(const unsigned short curr_x, const unsigned short curr_y, const unsigned short next_x, const unsigned short next_y)
{
	assert(curr_x == next_x || curr_y == next_y);
	assert(curr_x != next_x || curr_y != next_y);

	cell& cell_curr = cell_at(curr_x, curr_y);
	cell& cell_next = cell_at(next_x, next_y);

	//Wrapping
	if (next_x == 0 && curr_x == _size_cell - 1) {
		cell_curr.add_tube(cell::cs_right);
		cell_next.add_tube(cell::cs_left);
	}
	else if (next_x == _size_cell - 1 && curr_x == 0) {
		cell_curr.add_tube(cell::cs_left);
		cell_next.add_tube(cell::cs_right);
	}
	else if (next_y == 0 && curr_y == _size_cell - 1) {
		cell_curr.add_tube(cell::cs_bottom);
		cell_next.add_tube(cell::cs_top);
	}
	else if (next_y == _size_cell - 1 && curr_y == 0) {
		cell_curr.add_tube(cell::cs_top);
		cell_next.add_tube(cell::cs_bottom);
	}
	//Non-wrapping
	else if (next_x < curr_x) {
		cell_curr.add_tube(cell::cs_left);
		cell_next.add_tube(cell::cs_right);
	}
	else if (next_x > curr_x) {
		cell_curr.add_tube(cell::cs_right);
		cell_next.add_tube(cell::cs_left);
	}
	else if (next_y < curr_y) {
		cell_curr.add_tube(cell::cs_top);
		cell_next.add_tube(cell::cs_bottom);
	}
	else if (next_y > curr_y) {
		cell_curr.add_tube(cell::cs_bottom);
		cell_next.add_tube(cell::cs_top);
	}
}


void level::define_connect_status(const unsigned short x, const unsigned short y)
{
	cell& cell_curr = cell_at(x, y);
	if (cell_curr.active() || cell_curr.rotation_in_progress())
		return;	//Already connected or rotate in progress

	cell_curr.set_active(true);

	//to up
	if (cell_curr.top_connected()) {
		if (_wrap_mode || y > 0) {
			const unsigned short next_y = y > 0 ? y - 1 : _size_cell - 1;
			cell& cell_next = cell_at(x, next_y);
			if (cell_next.bottom_connected())
				define_connect_status(x, next_y);
		}
	}

	//to down
	if (cell_curr.bottom_connected()) {
		if (_wrap_mode || y < _size_cell - 1) {
			const unsigned short next_y = (y + 1) % _size_cell;
			cell& cell_next = cell_at(x, next_y);
			if (cell_next.top_connected())
				define_connect_status(x, next_y);
		}
	}

	//to left
	if (cell_curr.left_connected()) {
		if (_wrap_mode || x > 0) {
			const unsigned short next_x = x > 0 ? x - 1 : _size_cell - 1;
			cell& cell_next = cell_at(next_x, y);
			if (cell_next.right_connected())
				define_connect_status(next_x, y);
		}
	}

	//to right
	if (cell_curr.right_connected()) {
		if (_wrap_mode || x < _size_cell - 1) {
			const unsigned short next_x = (x + 1) % _size_cell;
			cell& cell_next = cell_at(next_x, y);
			if (cell_next.left_connected())
				define_connect_status(next_x, y);
		}
	}
}


void level::define_connect_status()
{
	//Reset connection status
	for (cells::iterator it = _cells.begin(); it != _cells.end(); ++it)
		it->set_active(false);

	define_connect_status(_senderX, _senderY);

	_solved = true;
	for (cells::const_iterator it = _cells.begin(); _solved && it != _cells.end(); ++it)
		_solved = !it->rotation_in_progress() && (it->cell_type() != cell::ct_receiver || it->active());

	if (_solved) {
		//Reset locks
		for (cells::iterator it = _cells.begin(); _solved && it != _cells.end(); ++it)
			if (it->locked())
				it->reverse_lock();
	}
}


unsigned short level::size_from_type(const size sz)
{
	switch (sz) {
		case sz_small:
			return 10;
		case sz_normal:
			return 14;
		case sz_big:
			return 20;
		case sz_extra:
			return 30;
		default:
			assert(false && "Unknown size");
	}
	return 14;
}


cell::state level::calculate_state()
{
	cell::state rs = cell::st_unchanged;

	for (cells::iterator it = _cells.begin(); it != _cells.end(); ++it) {
		const cell::state cs = it->calculate_state();
		if (cs == cell::st_updated)
			rs = cell::st_updated;
		else if (cs == cell::st_rcomplete && rs != cell::st_updated)
			rs = cell::st_rcomplete;
	}

	if (rs == cell::st_rcomplete)
		define_connect_status();

	return rs;
}


void level::reverse_lock(const unsigned short x, const unsigned short y)
{
	cell& c = cell_at(x, y);
	if (c.cell_type() != cell::ct_free)
		c.reverse_lock();
}


void level::rotate(const unsigned short x, const unsigned short y, const bool dir)
{
	cell& c = cell_at(x, y);
	if (c.cell_type() != cell::ct_free) {
		c.rotate(dir);
		define_connect_status();
	}
}
