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


CCell::CCell()
{
	Reset();
}


CCell::~CCell()
{
}


void CCell::Reset()
{
	//Reset state of the object to default values
	Type = ObjNone,
	State = false;
	ConnSide = CONNECT_UNDEF;
	RotateAngle = 0.0f;
	RotateAngleEnd = 0.0f;
	RotateCurr = 0.0f;
	RotateStart = 0;
	RotateTwice = false;
	Weight = MAX_WEIGHT;
	Used = false;
}


void CCell::SetAngleBySide()
{
	switch (Type) {
		case ObjNone:
			break;
		case ObjStrTube:
		case ObjCurTube:
			switch (ConnSide) {
				case (CONNECT_UP | CONNECT_DOWN):		RotateAngle = 0 * 90;	break;
				case (CONNECT_UP | CONNECT_RIGHT):		RotateAngle = 0 * 90;	break;
				case (CONNECT_RIGHT | CONNECT_DOWN):	RotateAngle = 1 * 90;	break;
				case (CONNECT_RIGHT | CONNECT_LEFT):	RotateAngle = 1 * 90;	break;
				case (CONNECT_DOWN | CONNECT_LEFT):		RotateAngle = 2 * 90;	break;
				case (CONNECT_LEFT | CONNECT_UP):		RotateAngle = 3 * 90;	break;
			}
			break;
		case ObjTubeJoint:
			switch (ConnSide) {
				case (CONNECT_UP | CONNECT_RIGHT | CONNECT_DOWN):	RotateAngle = 0 * 90;	break;
				case (CONNECT_RIGHT | CONNECT_DOWN | CONNECT_LEFT):	RotateAngle = 1 * 90;	break;
				case (CONNECT_DOWN | CONNECT_LEFT | CONNECT_UP):	RotateAngle = 2 * 90;	break;
				case (CONNECT_LEFT | CONNECT_UP | CONNECT_RIGHT):	RotateAngle = 3 * 90;	break;
			}
			break;
		case ObjReceiver:
		case ObjSender:
			switch (ConnSide) {
				case CONNECT_UP:	RotateAngle = 0 * 90;	break;
				case CONNECT_RIGHT:	RotateAngle = 1 * 90;	break;
				case CONNECT_DOWN:	RotateAngle = 2 * 90;	break;
				case CONNECT_LEFT:	RotateAngle = 3 * 90;	break;
			}
			break;
	}
}


void CCell::SetConnectionSideByRotate(bool fClockwise)
{
	if (fClockwise)
		ConnSide = (ConnSide << 1) | (ConnSide >> 3);
	else
		ConnSide = (ConnSide >> 1) | (ConnSide << 3);
	ConnSide &= 0xF;
}


void CCell::StartRotate(bool fClockwise, bool fTwice)
{
	if (Type == ObjNone)
		return;	//Nothing to rotate

	if (RotateStart != 0) {
		//Rotation already in progress
		RotateTwice = true;
		return;
	}
	RotateTwice = fTwice;
	RotateStart = GetTickCount();
	RotateAngleEnd = RotateAngle + (fClockwise ? 90.0f : -90.0f);
}
