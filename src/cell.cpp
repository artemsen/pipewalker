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

#include "cell.h"
#include "synchro.h"


CCell::CCell()
{
	Reset();
}


void CCell::Reset()
{
	_TubeType = TTNone;
	_CellType = CTFree;
	_Angle = 0.0f;
	_State = false;
	_Lock = false;
	_ConnTop = false;
	_ConnBottom = false;
	_ConnLeft = false;
	_ConnRight = false;
	_Used = false;
}


string CCell::Save() const
{
	const int rotate = static_cast<int>(IsRotationInProgress() ? _Rotate.InitAngle / 90.0f : _Angle / 90.0f);
	assert(rotate >= 0 && rotate <= 3);

	string state(4, 0);
	state[0] = '0' + static_cast<char>(_TubeType);
	state[1] = '0' + static_cast<char>(_CellType);
	state[2] = '0' + static_cast<char>(rotate);
	state[3] = '0' + (_Lock ? 1 : 0);
	return state;
}


bool CCell::Load(const string& state)
{
	if (state.length() != 4)	//Incorrect format?
		return false;

	_TubeType = static_cast<TubeType>(state[0] - '0');
	if (_TubeType < TTNone || _TubeType > TTJoiner)
		return false;
	_CellType = static_cast<CellType>(state[1] - '0');
	if (_CellType < CTFree || _CellType > CTReceiver)
		return false;
	const int rotate = state[2] - '0';
	if (rotate < 0 || rotate > 3)
		return false;
	_Lock = state[3] - '0' == 1;

	_Angle = rotate * 90.0f;

	//Restore connection side by rotate angle
	switch (rotate) {
		case 0:	//0 degrees
			switch (_TubeType) {
				case TTNone:														break;
				case TTHalf:		_ConnTop = true;								break;
				case TTStraight:	_ConnTop = _ConnBottom = true;				break;
				case TTCurved:		_ConnTop = _ConnRight = true;					break;
				case TTJoiner:		_ConnTop = _ConnBottom = _ConnRight = true;	break;
				default:			assert(false && "Unknown tube type"); return false;
			}
			break;
		case 1:	//90 degrees
			switch (_TubeType) {
				case TTNone:														break;
				case TTHalf:		_ConnLeft = true;								break;
				case TTStraight:	_ConnRight = _ConnLeft = true;				break;
				case TTCurved:		_ConnTop = _ConnLeft = true;					break;
				case TTJoiner:		_ConnTop = _ConnLeft = _ConnRight = true;	break;
				default:			assert(false && "Unknown tube type"); return false;
			}
			break;
		case 2:	//180 degrees
			switch (_TubeType) {
				case TTNone:														break;
				case TTHalf:		_ConnBottom = true;							break;
				case TTStraight:	_ConnTop = _ConnBottom = true;				break;
				case TTCurved:		_ConnBottom = _ConnLeft = true;				break;
				case TTJoiner:		_ConnTop = _ConnBottom = _ConnLeft = true;	break;
				default:			assert(false && "Unknown tube type"); return false;
			}
			break;
		case 3:	//270 degrees
			switch (_TubeType) {
				case TTNone:														break;
				case TTHalf:		_ConnRight = true;								break;
				case TTStraight:	_ConnRight = _ConnLeft = true;				break;
				case TTCurved:		_ConnRight = _ConnBottom = true;				break;
				case TTJoiner:		_ConnRight = _ConnLeft = _ConnBottom = true;	break;
				default:			assert(false && "Unknown tube type"); return false;
			}
			break;
		default:
			assert(false && "Wrong angle");
			return false;
	}

	return true;
}


void CCell::AddTube(const ConnectionSide side)
{
	assert(CanAddTube());

	//Already connected?
	assert(!(_ConnTop && side == CSTop));
	assert(!(_ConnBottom && side == CSBottom));
	assert(!(_ConnLeft && side == CSLeft));
	assert(!(_ConnRight && side == CSRight));

	//Add connection
	switch (side) {
		case CSTop:		_ConnTop = true;		break;
		case CSBottom:	_ConnBottom = true;	break;
		case CSLeft:	_ConnLeft = true;		break;
		case CSRight:	_ConnRight = true;		break;
		default:
			assert(false && "Undefined connection side");
			break;
	}
	//Define cell type
	if (_CellType == CTFree)
		_CellType = CTTube;

	//Define tube type
	switch (GetTubeSideCount()) {
		case 1:	_TubeType = TTHalf;	break;
		case 2:	_TubeType = (_ConnTop || _ConnBottom) && (_ConnLeft || _ConnRight) ? TTCurved : TTStraight;	break;
		case 3:	_TubeType = TTJoiner;	break;
		default:
			assert(false && "Incorrect connection count");
			break;
	}

	//Define angle from base direction (anticlockwise)
	_Angle = 0.0f;
	switch (_TubeType) {
		case TTHalf:
			if (_ConnRight)									_Angle = 90.0f * 3;
			else if (_ConnBottom)								_Angle = 90.0f * 2;
			else if (_ConnLeft)								_Angle = 90.0f * 1;
			break;
		case TTStraight:
			if (_ConnRight /*|| _ConnLeft*/)					_Angle = 90.0f * 1;
			break;
		case TTCurved:
			if (_ConnRight && _ConnBottom)					_Angle = 90.0f * 3;
			else if (_ConnBottom && _ConnLeft)				_Angle = 90.0f * 2;
			else if (_ConnLeft && _ConnTop)					_Angle = 90.0f * 1;
			break;
		case TTJoiner:
			if (_ConnLeft && _ConnRight && _ConnBottom)		_Angle = 90.0f * 3;
			else if (_ConnTop && _ConnBottom && _ConnLeft)	_Angle = 90.0f * 2;
			else if (_ConnLeft && _ConnRight && _ConnTop)	_Angle = 90.0f * 1;
			break;
		default:
			assert(false && "Unknown tube type");
			break;
	}
}


bool CCell::CanAddTube() const
{
	const unsigned char connectionCount = GetTubeSideCount();
	return
		_CellType == CTFree ||
		(_CellType == CTTube && connectionCount < 3) ||
		((_CellType == CTSender || _CellType == CTReceiver) && connectionCount == 0);
}


unsigned char CCell::GetTubeSideCount() const
{
	unsigned char connectionCount = 0;
	if (_ConnTop)		connectionCount++;
	if (_ConnBottom)	connectionCount++;
	if (_ConnLeft)		connectionCount++;
	if (_ConnRight)	connectionCount++;
	return connectionCount;
}


void CCell::Rotate(const bool dir)
{
	if (IsLocked())
		return;

	if (IsRotationInProgress()) {
		_Rotate.Twice = true;
		return;
	}

	_Rotate.Direction = dir;
	_Rotate.StartTime = CSynchro::GetTick();
	_Rotate.Twice = false;
	_Rotate.InitAngle = _Angle;
	_Rotate.NeedAngle = _Rotate.InitAngle + (_Rotate.Direction ? -90.0f : 90.0f);
}


bool CCell::ProcessRotation()
{
	if (!IsRotationInProgress())
		return false;

	float degree;
	if (CSynchro::GetPhase(_Rotate.StartTime, 300, degree)) {
		degree *= 90.0f;
		_Angle = (_Rotate.Direction ? -degree : degree);
		_Angle += _Rotate.InitAngle;
		return false;
	}

	_Angle = _Rotate.NeedAngle;
	if (_Angle > 359.0f)
		_Angle = 0.0f;
	if (_Angle < 0.0f)
		_Angle = 270.0f;
	_Rotate.StartTime = 0;

	//Calculate new connection sides
	bool newConnTop = false, newConnBottom = false, newConnLeft = false, newConnRight = false;
	if (_Rotate.Direction) {
		newConnTop = _ConnLeft;
		newConnBottom = _ConnRight;
		newConnLeft = _ConnBottom;
		newConnRight = _ConnTop;
	}
	else {
		newConnTop = _ConnRight;
		newConnBottom = _ConnLeft;
		newConnLeft = _ConnTop;
		newConnRight = _ConnBottom;
	}
	_ConnTop = newConnTop;
	_ConnBottom = newConnBottom;
	_ConnLeft = newConnLeft;
	_ConnRight = newConnRight;


	if (_Rotate.Twice) {
		Rotate(_Rotate.Direction);
		return false;
	}

	return true;
}
