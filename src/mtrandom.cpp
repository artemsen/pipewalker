/**************************************************************************
 *  PipeWalker game (http://pipewalker.sourceforge.net)                   *
 *  Copyright (C) 2007-2010 by Artem A. Senichev <artemsen@gmail.com>     *
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

unsigned long	CMTRandom::_State[MTSTATE_ARRAY_SIZE];
unsigned long	CMTRandom::_Pos = 0;
bool			CMTRandom::_Initialized = false;

#define MTRAND	397


void CMTRandom::Seed(const unsigned long seed)
{
	_State[0] = seed & 0xFFFFFFFFUL;	//For > 32 bit machines
	for (unsigned long i = 1; i < MTSTATE_ARRAY_SIZE; ++i) {
		_State[i] = 1812433253UL * (_State[i - 1] ^ (_State[i - 1] >> 30)) + i;
		_State[i] &= 0xFFFFFFFFUL; //For > 32 bit machines
	}
	_Pos = MTSTATE_ARRAY_SIZE; //Force GenerateState() to be called for next random number
}


unsigned long CMTRandom::Rand()
{
	if (!_Initialized) {
		Seed(0xabcdef);
		_Initialized = true;
	}

	if (_Pos == MTSTATE_ARRAY_SIZE)
		GenerateState(); // new state vector needed
	unsigned long x = _State[_Pos++];
	x ^= (x >> 11);
	x ^= (x << 7) & 0x9D2C5680UL;
	x ^= (x << 15) & 0xEFC60000UL;
	return (x ^ (x >> 18));
}


unsigned long CMTRandom::Twiddle(const unsigned long u, const unsigned long v)
{
	return (((u & 0x80000000UL) | (v & 0x7FFFFFFFUL)) >> 1) ^ ((v & 1UL) ? 0x9908B0DFUL : 0x0UL);
}


void CMTRandom::GenerateState()
{
	for (int i = 0; i < (MTSTATE_ARRAY_SIZE - MTRAND); ++i)
		_State[i] = _State[i + MTRAND] ^ Twiddle(_State[i], _State[i + 1]);
	for (int i = MTSTATE_ARRAY_SIZE - MTRAND; i < (MTSTATE_ARRAY_SIZE - 1); ++i)
		_State[i] = _State[i + MTRAND - MTSTATE_ARRAY_SIZE] ^ Twiddle(_State[i], _State[i + 1]);
	_State[MTSTATE_ARRAY_SIZE - 1] = _State[MTRAND - 1] ^ Twiddle(_State[MTSTATE_ARRAY_SIZE - 1], _State[0]);
	_Pos = 0;	//Reset position
}
