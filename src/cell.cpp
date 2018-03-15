/************************************************************************** 
 *  PipeWalker - simple puzzle game                                       * 
 *  Copyright (C) 2007-2008 by Artem A. Senichev <artemsen@gmail.com>     * 
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


CCell::CCell(void)
{
	Reset();
}


CCell::~CCell()
{
}


CCell& CCell::operator=(const CCell& cell) 
{
	Reset();
	Type = cell.Type;
	Sides = cell.Sides;
	return *this;
}


void CCell::Reset()
{
	//Reset state of the object to default values
	Type = Free,
	State = false;
	m_fLock = false;
	Sides = CONNECT_UNDEF;
	Weight = MAX_WEIGHT;
	Used = false;

	Rotate.StartTime = 0;
	Rotate.Angle = 0.0f;
	Rotate.Twice = false;
}


void CCell::StartRotate(const Direction enuDir)
{
	assert(Type != Free);	//Nothing to rotate

	if (IsRotationInProgress()) {
		//Rotation already in progress
		Rotate.Twice = true;
		return;
	}

	Rotate.Dir = enuDir;
	Rotate.Twice = false;
	Rotate.StartTime = GetTickCount();
}


void CCell::EndRotate(void)
{
	assert(IsRotationInProgress());

	Rotate.StartTime = 0;
	Rotate.Angle = 0.0f;

	//Define new connected sides
	unsigned int unNewSides = 0;
	if (Sides & CONNECT_UP)
		unNewSides |= (Rotate.Dir == Positive ? CONNECT_RIGHT : CONNECT_LEFT);
	if (Sides & CONNECT_DOWN)
		unNewSides |= (Rotate.Dir == Positive ? CONNECT_LEFT : CONNECT_RIGHT);
	if (Sides & CONNECT_LEFT)
		unNewSides |= (Rotate.Dir == Positive ? CONNECT_UP : CONNECT_DOWN);
	if (Sides & CONNECT_RIGHT)
		unNewSides |= (Rotate.Dir == Positive ? CONNECT_DOWN : CONNECT_UP);
#ifndef NDEBUG
	char szRotSt[5];
	szRotSt[0] = Sides & CONNECT_UP ? 'U' : '-';
	szRotSt[1] = Sides & CONNECT_DOWN ? 'D' : '-';
	szRotSt[2] = Sides & CONNECT_LEFT ? 'L' : '-';
	szRotSt[3] = Sides & CONNECT_RIGHT ? 'R' : '-';
	szRotSt[4] = 0;
	char szRotFn[5];
	szRotFn[0] = unNewSides & CONNECT_UP ? 'U' : '-';
	szRotFn[1] = unNewSides & CONNECT_DOWN ? 'D' : '-';
	szRotFn[2] = unNewSides & CONNECT_LEFT ? 'L' : '-';
	szRotFn[3] = unNewSides & CONNECT_RIGHT ? 'R' : '-';
	szRotFn[4] = 0;
	printf("Rotation: %s -> %s\n", szRotSt, szRotFn);
#endif	//NDEBUG
	Sides = unNewSides;

	if (Rotate.Twice)
		StartRotate(Rotate.Dir);
}


unsigned short CCell::GetSideCount(void) const
{
	unsigned short nCount = 0;
	for (unsigned int i = 0; i < sizeof(unsigned int) * 8 /*bits per byte*/; i++) {
		if (Sides & (1 << i))
			nCount++;
	}
	return nCount;
}


bool CCell::IsCurved(void) const
{
	return (
		GetSideCount() > 2 ||
		((Sides & CONNECT_UP || Sides & CONNECT_DOWN) && (Sides & CONNECT_LEFT || Sides & CONNECT_RIGHT))
		);
}
