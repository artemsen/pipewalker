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

#include "mtrandom.h"

#define MTR_RAND	397
#define MTR_TWIDDLE(u, v) ((((u & 0x80000000UL) | (v & 0x7fffffffUL)) >> 1) ^ ((v & 1UL) ? 0x9908b0dfUL : 0x0UL))


mtrandom::mtrandom()
: _pos(0)
{
}


mtrandom& mtrandom::instance()
{
	static mtrandom i;
	return i;
}


void mtrandom::seed(const unsigned long seed)
{
	mtrandom& inst = instance();

	inst._state[0] = seed & 0xffffffffUL;	//For > 32 bit machines
	for (unsigned long i = 1; i < MTR_STATE_ARRAY_SIZE; ++i) {
		inst._state[i] = 1812433253UL * (inst._state[i - 1] ^ (inst._state[i - 1] >> 30)) + i;
		inst._state[i] &= 0xffffffffUL;		//For > 32 bit machines
	}
	inst._pos = MTR_STATE_ARRAY_SIZE; //Force generate_state() to be called for next random number
}


float mtrandom::random(const float min_val, const float max_val)
{
	const float r = min_val + static_cast<float>(instance().random() % static_cast<unsigned long>(((max_val - min_val) * 1000.f))) / 1000.0f;
	assert(r >= min_val && r <= max_val);
	return r;

}


int mtrandom::random(const int min_val, const int max_val)
{
	const int r = min_val + (instance().random() % (max_val - min_val));
	assert(r >= min_val && r <= max_val);
	return r;

}


void mtrandom::generate_state()
{
	for (int i = 0; i < (MTR_STATE_ARRAY_SIZE - MTR_RAND); ++i)
		_state[i] = _state[i + MTR_RAND] ^ MTR_TWIDDLE(_state[i], _state[i + 1]);
	for (int i = MTR_STATE_ARRAY_SIZE - MTR_RAND; i < (MTR_STATE_ARRAY_SIZE - 1); ++i)
		_state[i] = _state[i + MTR_RAND - MTR_STATE_ARRAY_SIZE] ^ MTR_TWIDDLE(_state[i], _state[i + 1]);
	_state[MTR_STATE_ARRAY_SIZE - 1] = _state[MTR_RAND - 1] ^ MTR_TWIDDLE(_state[MTR_STATE_ARRAY_SIZE - 1], _state[0]);
	_pos = 0;	//Reset position
}


unsigned long mtrandom::random()
{
	if (_pos == MTR_STATE_ARRAY_SIZE)
		generate_state(); //New state vector needed
	unsigned long x = _state[_pos++];
	x ^= (x >> 11);
	x ^= (x << 7) & 0x9d2c5680UL;
	x ^= (x << 15) & 0xefc60000UL;
	return (x ^ (x >> 18));
}
